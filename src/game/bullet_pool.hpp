#pragma once

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/sprite_frames.hpp> // 引入 SpriteFrames
#include <godot_cpp/variant/rid.hpp>
#include <godot_cpp/variant/rect2.hpp>
#include <godot_cpp/variant/string_name.hpp>

#include <vector>
#include <functional>

namespace game {

struct BulletConfig {
    godot::Ref<godot::SpriteFrames> sprite_frames;
    godot::StringName anim_name = "default";
    float radius = 4.0f;
    
    // 以下缓存字段用于避免每帧去重构的 SpriteFrames 里查找，提升性能
    int total_frames = 1;
    float anim_speed = 10.0f;
    bool loop = true;
};

class BulletPool : public godot::Node2D {
    GDCLASS(BulletPool, Node2D)
public:
    struct Bullet {
        godot::RID shape_rid;
        godot::RID canvas_item_rid;
        
        godot::Vector2 position;
        godot::Vector2 velocity;
        godot::Vector2 base_velocity; 
        float rotation = 0.0f;
        float lifetime = 0.0f; 
        bool active = false;

        BulletConfig config;
        float anim_timer = 0.0f;
        int current_frame = -1; // 默认 -1，用于脏检查（帧改变时才清空重绘）

        std::function<void(Bullet&)> behavior_fn;
    };

private:
    static BulletPool* new_bullet_pool;
    std::vector<Bullet> pool;
    size_t pool_size = 5000;
  
protected:
    static void _bind_methods();
  
public:
    BulletPool();
    ~BulletPool();
    static BulletPool* get_pool();
    void _ready();
    void _physics_process(double delta);
  
    // 修改后的 spawn 接口
    void spawn(godot::Vector2 p_pos, 
               std::function<void(Bullet&)> p_behavior, 
               float p_rot, 
               godot::Ref<godot::SpriteFrames> p_sprite_frames, 
               godot::StringName p_anim_name,
               float p_radius);
    void spawn_static(godot::Vector2 p_pos, 
                      std::function<void(Bullet&)> p_behavior, 
                      float p_rot, 
                      godot::Ref<godot::Texture2D> p_texture, // 直接接收图片
                      float p_radius);
    
    void recycle_bullet(Bullet &bullet);
};
}