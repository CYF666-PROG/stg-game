#include "audio_manager.hpp"
#include "godot_cpp/classes/video_stream_player.hpp"
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/time.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/window.hpp>

using namespace godot;
using namespace game;

AudioManager *AudioManager::singleton = nullptr;

void AudioManager::_bind_methods() {
  ClassDB::bind_method(D_METHOD("play", "sound_name"), &AudioManager::play);
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
  // 初始化对象池
  for (int i = 0; i < pool_size; ++i) {
    AudioStreamPlayer *player = memnew(AudioStreamPlayer);
    add_child(player);
    player_pool.push_back(player);
  }

  bgm_player = memnew(AudioStreamPlayer);
  add_child(bgm_player);
  bgm_player->set_bus("Music"); // 绑定到 Music 总线

  // 预载入音效并设置 初始音量 与 冷却
  ResourceLoader *loader = ResourceLoader::get_singleton();
  // 子弹音效
  SoundSetting bullet_set;
  bullet_set.stream = loader->load("res://material/audio/shoot/bong00.wav");
  bullet_set.default_volume_db = -40.0; 
  bullet_set.cooldown_ms = 40; // 子弹高频，限流 40ms
  sound_registry["bong00"] = bullet_set;
  // 快速射击音效
  SoundSetting shoot_fast;
  shoot_fast.stream = loader->load("res://material/audio/shoot/fast.wav");
  shoot_fast.default_volume_db = -35.0; 
  shoot_fast.cooldown_ms = 20; // 限制极短时间内的重叠
  sound_registry["fast"] = shoot_fast;
  // miss音效
  SoundSetting player_dead;
  player_dead.stream = loader->load("res://material/audio/player_dead.wav");
  player_dead.default_volume_db = -20; 
  player_dead.cooldown_ms = 200;
  sound_registry["player_dead"] = player_dead;
  // 小怪死亡音效
  SoundSetting imp_dead;
  imp_dead.stream = loader->load("res://material/audio/imp_dead.wav");
  imp_dead.default_volume_db = -30; 
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
  boos_end.default_volume_db = -35; 
  boos_end.cooldown_ms = 100;
  sound_registry["boos_end"] = boos_end;
  // 注册BGM
  bgm_registry["boss_nitori"] = loader->load("res://material/audio/song/candid_friend.mp3");
}

void AudioManager::set_sound_volume(const StringName &sound_name, double p_volume_db) {
  if (sound_registry.has(sound_name)) {
    sound_registry[sound_name].default_volume_db = p_volume_db;
  }
}

void AudioManager::play(const StringName &sound_name) {
  if (!sound_registry.has(sound_name)) {
    return;
  }

  const SoundSetting &setting = sound_registry[sound_name];
  uint64_t current_time = Time::get_singleton()->get_ticks_msec();
  
  // 检查是否在 冷却时间 内重复调用
  bool in_cooldown = false;
  if (last_played_time.has(sound_name)) {
    if (current_time - last_played_time[sound_name] < setting.cooldown_ms) {
      in_cooldown = true; // 触发频率过高，进入覆盖逻辑
    }
  }
  last_played_time[sound_name] = current_time;

  AudioStreamPlayer *target_player = nullptr;

  // 覆盖逻辑
  if (in_cooldown && last_assigned_player.has(sound_name)) {
    // 直接获取上一次播放这个音效的播放器 并终止
    target_player = Object::cast_to<AudioStreamPlayer>(last_assigned_player[sound_name]);
    if (target_player) {
      target_player->stop();
    }
  }
  // 如果不在限制时间内 或上一次的播放器失效 就重新获取一个播放器
  if (!target_player) {
    target_player = player_pool[current_index];
    current_index = (current_index + 1) % pool_size;
  }
  // 记录这次这个音效捆绑给了哪个播放器，供下一次 覆盖 时提取
  last_assigned_player[sound_name] = target_player;
  // 执行播放
  if (target_player && !setting.stream.is_null()) {
    // 微弱的音高变化
    double random_pitch = UtilityFunctions::randf_range(0.97, 1.03); 
    target_player->set_stream(setting.stream);
    target_player->set_volume_db(setting.default_volume_db);
    target_player->set_pitch_scale(random_pitch);
    target_player->play();
  }
}

void AudioManager::_play_by_stream(const Ref<AudioStream> &stream, double volume_db, double pitch_min, double pitch_max) {
  if (stream.is_null()) return;
  AudioStreamPlayer *player = player_pool[current_index];
  // 如果播放器正在播放 直接 stop() 换歌会有点问题
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

// 播放指定的背景音乐
void AudioManager::play_bgm(const StringName &bgm_name, bool loop) {
  // 先暂停背景视频的声音
  godot::SceneTree* tree = get_tree();
  if (tree == nullptr) {
    godot::UtilityFunctions::push_error("无法获取 SceneTree！");
    return ;
  }
  godot::Window* root = tree->get_root();
  if (root == nullptr) {
    godot::UtilityFunctions::push_error("无法获取根节点 Root！");
    return ;
  }
  // 使用 get_node 并通过 Object::cast_to 进行安全的类型转换
  godot::Node* found_node = root->get_node_or_null("/root/play/VideoStreamPlayer");
  if (found_node == nullptr) {
    godot::UtilityFunctions::push_warning("not find /root/play/VideoStreamPlayer");
    return ;
  }
  auto* video_player = Object::cast_to<VideoStreamPlayer>(found_node);
  if (video_player == nullptr) {
    godot::UtilityFunctions::push_error("video_player erro");
    return;
  }
  video_player->set_volume_db(-80.0);

  // 如果新传入的歌正在播放 直接返回
  if (current_bgm_name == bgm_name && bgm_player && bgm_player->is_playing()) {
    return;
  }
  // 检查 BGM 注册表
  if (!bgm_registry.has(bgm_name)) {
    UtilityFunctions::push_error("BGM not find " + String(bgm_name));
    return;
  }

  current_bgm_name = bgm_name;
  Ref<AudioStream> stream = bgm_registry[bgm_name];

  if (bgm_player && !stream.is_null()) {
    bgm_player->stop(); // 先停止当前歌曲
    bgm_player->set_stream(stream);
    bgm_player->set_pitch_scale(1.0); // BGM 严禁变音高
    
    bgm_player->play();
  }
}

void AudioManager::stop_bgm() {
  if (bgm_player && bgm_player->is_playing()) {
    bgm_player->stop();
    current_bgm_name = "";
  }
}

void AudioManager::pause_bgm() {
  if (bgm_player && bgm_player->is_playing()) {
    bgm_player->set_stream_paused(true);
  }
}

void AudioManager::resume_bgm() {
  if (bgm_player && bgm_player->get_stream_paused()) {
    bgm_player->set_stream_paused(false);
  }
}

void AudioManager::set_bgm_volume(double volume_db) {
  if (bgm_player) {
    bgm_player->set_volume_db(volume_db);
  }
}