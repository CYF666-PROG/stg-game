#include "audio_manager.hpp"
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

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

    // 2. 预载入音效并设置【专属初始音量】
    ResourceLoader *loader = ResourceLoader::get_singleton();
    
    // 子弹音效：高频且刺耳，初始音量压低到 -8.0 dB
    SoundSetting bullet_set;
    bullet_set.stream = loader->load("res://material/audio/shoot/se_tan00.wav");
    bullet_set.default_volume_db = -20.0; 
    sound_registry["bullet00"] = bullet_set;

    // miss音效
    SoundSetting player_dead;
    player_dead.stream = loader->load("res://material/audio/player_dead.wav");
    player_dead.default_volume_db = -10; 
    sound_registry["player_dead"] = player_dead;

    // 小怪死亡音效
    SoundSetting imp_dead;
    imp_dead.stream = loader->load("res://material/audio/imp_dead.wav");
    imp_dead.default_volume_db = -20; 
    sound_registry["imp_dead"] = imp_dead;
}

// 动态修改某个音效音量的接口
void AudioManager::set_sound_volume(const StringName &sound_name, double p_volume_db) {
    if (sound_registry.has(sound_name)) {
        sound_registry[sound_name].default_volume_db = p_volume_db;
    }
}

// 专属高频子弹接口
void AudioManager::fire_bullet() {
    StringName sound_key = "bullet00";
    
    // 限流拦截
    uint64_t current_time = Time::get_singleton()->get_ticks_msec();
    if (last_played_time.has(sound_key)) {
        if (current_time - last_played_time[sound_key] < min_fire_interval_ms) {
            return; 
        }
    }
    last_played_time[sound_key] = current_time;

    // 自动读取该音效注册的专属音量 (-8.0 dB)
    if (sound_registry.has(sound_key)) {
        const SoundSetting &setting = sound_registry[sound_key];
        _play_by_stream(setting.stream, setting.default_volume_db, 0.92, 1.08);
    }
}

// 通用播放接口
void AudioManager::play(const StringName &sound_name) {
    if (sound_registry.has(sound_name)) {
        const SoundSetting &setting = sound_registry[sound_name];
        // 自动读取该音效注册的专属音量，不带随机音高 (1.0, 1.0)
        _play_by_stream(setting.stream, setting.default_volume_db, 1.0, 1.0);
    }
}

// 核心池播放器状态切换
void AudioManager::_play_by_stream(const Ref<AudioStream> &stream, double volume_db, double pitch_min, double pitch_max) {
    if (stream.is_null()) return;

    AudioStreamPlayer *player = player_pool[current_index];
    if (player->is_playing()) {
        player->stop(); 
    }

    double random_pitch = UtilityFunctions::randf_range(pitch_min, pitch_max);

    player->set_stream(stream);
    player->set_volume_db(volume_db); // 完美的音量应用
    player->set_pitch_scale(random_pitch);
    player->play();

    current_index = (current_index + 1) % pool_size;
}