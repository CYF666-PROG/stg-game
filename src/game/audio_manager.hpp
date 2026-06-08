#pragma once

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/audio_stream_player.hpp>
#include <godot_cpp/classes/audio_stream.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/time.hpp>

namespace game {

// 新增：音效独立配置结构体
struct SoundSetting {
    godot::Ref<godot::AudioStream> stream;
    double default_volume_db = 0.0; // 每个音效专属的初始音量
};

class AudioManager : public godot::Node {
    GDCLASS(AudioManager, Node);

private:
    static AudioManager *singleton;
    
    godot::Vector<godot::AudioStreamPlayer*> player_pool;
    int pool_size = 32;
    int current_index = 0;

    // 升级：存储结构体，包含资源和音量
    godot::HashMap<godot::StringName, SoundSetting> sound_registry;
    godot::HashMap<godot::StringName, uint64_t> last_played_time;
    uint64_t min_fire_interval_ms = 100; 

    void _play_by_stream(const godot::Ref<godot::AudioStream> &stream, double volume_db, double pitch_min, double pitch_max);

protected:
    static void _bind_methods();

public:
    AudioManager();
    ~AudioManager();

    static AudioManager *get_audio();
    void _ready() override;

    // 核心播放接口
    void play(const godot::StringName &sound_name);
    void fire_bullet();                     
    
    // 新增：方便后续动态修改某个特定音效初始音量的接口
    void set_sound_volume(const godot::StringName &sound_name, double p_volume_db);
};

} // namespace godot