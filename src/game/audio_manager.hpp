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
    uint64_t cooldown_ms = 30; // 该音效专属的限流冷却时间（毫秒），默认 30ms 即可有效防破音
};

class AudioManager : public godot::Node {
    GDCLASS(AudioManager, Node);

private:
    // ─── 新增 BGM 相关私有成员 ───
    godot::AudioStreamPlayer* bgm_player = nullptr;
    
    // BGM 注册表：只存 StringName -> AudioStream 即可
    godot::HashMap<godot::StringName, godot::Ref<godot::AudioStream>> bgm_registry;
    
    // 记录当前播放的 BGM 名字，方便逻辑判断
    godot::StringName current_bgm_name = "";
    static AudioManager *singleton;
    // 🌟 新增：记录每个音效最后一次分配给哪一个播放器
    godot::Dictionary last_assigned_player;
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
    
    /// @brief 播放指定名称的背景音乐
    /// @param bgm_name 注册过的 BGM 名字
    /// @param loop 是否循环播放（Godot4 中通常在资源本身设置循环，但代码可以双重保险）
    void play_bgm(const godot::StringName &bgm_name, bool loop = true);
    
    /// @brief 停止当前背景音乐
    void stop_bgm();

    /// @brief 暂停背景音乐
    void pause_bgm();
    /// @brief 调节背景音乐音量
    /// @param volume_db 音量（分贝，0.0 为原音，负数变小）
    void set_bgm_volume(double volume_db);
    /// @brief 恢复播放背景音乐
    void resume_bgm();

    
    // 核心播放接口
    void play(const godot::StringName &sound_name);
    void fire_bullet();                     
    
    // 新增：方便后续动态修改某个特定音效初始音量的接口
    void set_sound_volume(const godot::StringName &sound_name, double p_volume_db);
};

} // namespace godot