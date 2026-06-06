#pragma once

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/sprite_frames.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <functional>

namespace game {

class EffectManager : public godot::Node2D {
    GDCLASS(EffectManager, godot::Node2D);

public:
    // 前置声明结构体
    struct EffectInstance;
    
    // Lambda 函数签名：
    // 参数 1: 当前特效实例的引用，供你自由修改 rotation, scale, color, current_frame, ticks_per_frame 等属性。
    // 参数 2: 该特效进行的总逻辑帧数
    // 返回值: bool，false 代表立即提前销毁该特效
    using EffectUpdateCallback = std::function<bool(EffectInstance& p_instance, int p_elapsed_ticks)>;

    // 完整的特效实例结构体
    struct EffectInstance {
        godot::RID canvas_item;                  // 底层硬件 RID
        godot::Ref<godot::SpriteFrames> sprite_frames;
        godot::StringName anim_name;
        
        // --- 可供 Lambda 自由修改的公共状态 ---
        int current_frame = 0;
        int tick_counter = 0;
        int ticks_per_frame = 1;
        /// @brief 生成后总帧数
        int total_elapsed_ticks = 0;
        bool loop = false;

        // --- 用于每帧控制的 3 大状态 ---
        // 特效创建时的初始全局坐标
        godot::Vector2 base_position;
        /// 缩放因子
        godot::Vector2 scale = godot::Vector2(1.0, 1.0);
        /// 旋转 rad
        double rotation = 0.0;
        // 颜色调制（含透明度）
        godot::Color modulate = godot::Color(1.0, 1.0, 1.0, 1.0);
        // 存储动态行为的回调
        EffectUpdateCallback update_callback = nullptr; 
    };

private:
    godot::Vector<EffectInstance> active_effects;

protected:
    static void _bind_methods();

public:
    static EffectManager *singleton;
    static EffectManager *get_singleton() { return singleton; }
    EffectManager();
    ~EffectManager();

    void _physics_process(double delta) override;

    // 特效贴图 生成位置 动画名 播放速度 回调 回调传入效果结构体EffectInstance 和存活帧数
    void spawn_effect(
        const godot::Ref<godot::SpriteFrames> &p_frames, 
        godot::Vector2 p_global_pos, 
        godot::StringName p_anim_name = "default", 
        double p_speed_scale = 1.0,
        EffectUpdateCallback p_callback = nullptr
    );
};

} // namespace game