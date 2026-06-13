#include "audio_manager.hpp"
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/time.hpp> // 确保引入了 Time 类

using namespace godot;
using namespace game;

AudioManager *AudioManager::singleton = nullptr;

void AudioManager::_bind_methods() {
    ClassDB::bind_method(D_METHOD("play", "sound_name"), &AudioManager::play);
    ClassDB::bind_method(D_METHOD("fire_bullet"), &AudioManager::fire_bullet);
    ClassDB::bind_method(D_METHOD("set_sound_volume", "sound_name", "volume_db"), &AudioManager::set_sound_volume);
}

AudioManager::AudioManager() {
    if (singleton == nullptr) singleton = this;
}

AudioManager::~AudioManager() {
    if (singleton == this) singleton = nullptr;
}

AudioManager *AudioManager::get_audio() { return singleton; }

void AudioManager::_ready() {
    // 1. 初始化对象池
    for (int i = 0; i < pool_size; ++i) {
        AudioStreamPlayer *player = memnew(AudioStreamPlayer);
        add_child(player);
        player_pool.push_back(player);
    }

    // 2. 预载入音效并设置【专属初始音量与冷却】
    ResourceLoader *loader = ResourceLoader::get_singleton();
    
    // 子弹音效
    SoundSetting bullet_set;
    bullet_set.stream = loader->load("res://material/audio/shoot/bong00.wav");
    bullet_set.default_volume_db = -30.0; 
    bullet_set.cooldown_ms = 40; // 子弹高频，限流 40ms
    sound_registry["bong00"] = bullet_set;

    // 快速射击音效 (🌟 修正：原本你这里错误地使用了 bullet_set，导致 fast 没注册成功)
    SoundSetting shoot_fast;
    shoot_fast.stream = loader->load("res://material/audio/shoot/fast.wav");
    shoot_fast.default_volume_db = -30.0; 
    shoot_fast.cooldown_ms = 20; // 限制极短时间内的重叠
    sound_registry["fast"] = shoot_fast;

    // miss音效
    SoundSetting player_dead;
    player_dead.stream = loader->load("res://material/audio/player_dead.wav");
    player_dead.default_volume_db = -10; 
    player_dead.cooldown_ms = 200; // 死亡音效不需要频繁触发
    sound_registry["player_dead"] = player_dead;

    // 小怪死亡音效
    SoundSetting imp_dead;
    imp_dead.stream = loader->load("res://material/audio/imp_dead.wav");
    imp_dead.default_volume_db = -20; 
    imp_dead.cooldown_ms = 45; // 🌟 关键：小怪成群死亡时，限制在 45ms 内只发出一次声音，绝不爆音
    sound_registry["imp_dead"] = imp_dead;
    // 符卡开始音效
    SoundSetting card_start;
    card_start.stream = loader->load("res://material/audio/card_start.wav");
    card_start.default_volume_db = -20; 
    card_start.cooldown_ms = 100;
    sound_registry["card_start"] = card_start;
    // boos结束音效
    SoundSetting boos_end;
    boos_end.stream = loader->load("res://material/audio/boos_end.wav");
    boos_end.default_volume_db = -20; 
    boos_end.cooldown_ms = 100;
    sound_registry["boos_end"] = card_start;
}

void AudioManager::set_sound_volume(const StringName &sound_name, double p_volume_db) {
    if (sound_registry.has(sound_name)) {
        sound_registry[sound_name].default_volume_db = p_volume_db;
    }
}

// 专属高频子弹接口（保留你原本的特定逻辑）
void AudioManager::fire_bullet() {
    StringName sound_key = "bong00";
    
    uint64_t current_time = Time::get_singleton()->get_ticks_msec();
    if (last_played_time.has(sound_key)) {
        if (current_time - last_played_time[sound_key] < min_fire_interval_ms) {
            return; 
        }
    }
    last_played_time[sound_key] = current_time;

    if (sound_registry.has(sound_key)) {
        const SoundSetting &setting = sound_registry[sound_key];
        _play_by_stream(setting.stream, setting.default_volume_db, 0.92, 1.08);
    }
}

// 🌟 通用播放接口（已升级防破音机制）
void AudioManager::play(const StringName &sound_name) {
    if (!sound_registry.has(sound_name)) {
        return;
    }

    const SoundSetting &setting = sound_registry[sound_name];
    uint64_t current_time = Time::get_singleton()->get_ticks_msec();
    
    // 检查是否在限制时间（冷却时间）内重复调用
    bool in_cooldown = false;
    if (last_played_time.has(sound_name)) {
        if (current_time - last_played_time[sound_name] < setting.cooldown_ms) {
            in_cooldown = true; // 触发频率过高，进入覆盖逻辑
        }
    }
    last_played_time[sound_name] = current_time;

    AudioStreamPlayer *target_player = nullptr;

    // 🔄 核心覆盖逻辑
    if (in_cooldown && last_assigned_player.has(sound_name)) {
        // 如果在限制时间内，直接抓取上一次播放这个音效的播放器
        target_player = Object::cast_to<AudioStreamPlayer>(last_assigned_player[sound_name]);
        
        if (target_player) {
            target_player->stop(); // 🔴 强行中断上一次还没播完的声音，防止波形叠加破音
        }
    }

    // 如果不在限制时间内，或者上一次的播放器失效了，就从对象池里轮询一个闲置/新的播放器
    if (!target_player) {
        target_player = player_pool[current_index];
        current_index = (current_index + 1) % pool_size;
    }

    // 记录这次这个音效捆绑给了哪个播放器，供下一次“覆盖”时提取
    last_assigned_player[sound_name] = target_player;

    // 🎵 执行播放
    if (target_player && !setting.stream.is_null()) {
        // 允许微弱的音高变化让音效更自然
        double random_pitch = UtilityFunctions::randf_range(0.97, 1.03); 
        
        target_player->set_stream(setting.stream);
        target_player->set_volume_db(setting.default_volume_db);
        target_player->set_pitch_scale(random_pitch);
        target_player->play();
    }
}

// 核心池播放器状态切换
void AudioManager::_play_by_stream(const Ref<AudioStream> &stream, double volume_db, double pitch_min, double pitch_max) {
    if (stream.is_null()) return;

    AudioStreamPlayer *player = player_pool[current_index];
    
    // 如果播放器正在播放，直接 stop() 换歌有时会产生微小的门限卡顿声
    if (player->is_playing()) {
        player->stop(); 
    }

    double random_pitch = UtilityFunctions::randf_range(pitch_min, pitch_max);

    player->set_stream(stream);
    player->set_volume_db(volume_db); 
    player->set_pitch_scale(random_pitch);
    player->play();

    current_index = (current_index + 1) % pool_size;
}