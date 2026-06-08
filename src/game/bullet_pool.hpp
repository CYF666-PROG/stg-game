#pragma once

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/sprite_frames.hpp> 
#include <godot_cpp/variant/rid.hpp>
#include <godot_cpp/variant/rect2.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <godot_cpp/classes/atlas_texture.hpp>

#include <vector>
#include <functional>

namespace game {

struct BulletConfig {
    godot::Ref<godot::SpriteFrames> sprite_frames;
    godot::StringName anim_name = "default";
    float radius = 4.0f;
    
    // ==================== 【新增参数】 ====================
    godot::Vector2 scale = godot::Vector2(1.0f, 1.0f);  // 贴图缩放
    godot::Vector2 anchor = godot::Vector2(0.5f, 0.5f); // 锚点 (0.0=左/上, 0.5=中心, 1.0=右/下)
    // ====================================================

    // 缓存字段，避免每帧查询 SpriteFrames 提升性能
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
        /// @brief 每帧速度
        godot::Vector2 velocity;
        double rotation = 0.0f;
        /// @brief 存活帧数
        int lifetime = 0; 
        bool active = false;
        /// @brief 碰撞层遮罩
        int mask = 1 ;
        BulletConfig config;
        float anim_timer = 0.0f;
        int current_frame = -1; 

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
  
    // 修改后的 spawn 接口，追加了 p_scale 和 p_anchor
    /// 初始坐标 运动函数 贴图 动画名称 判定点半径 碰撞层遮罩3为对第1,2层 z轴索引 缩放 锚点 初始角度rad 
    void spawn(godot::Vector2 p_pos, 
               std::function<void(Bullet&)> p_behavior, 
               godot::Ref<godot::SpriteFrames> p_sprite_frames, 
               godot::StringName p_anim_name,
               float p_radius,
               int mask = 1,
               int z_index = 10,
               godot::Vector2 p_scale = godot::Vector2(1.0f, 1.0f),
               godot::Vector2 p_anchor = godot::Vector2(0.5f, 0.5f),
               float p_rot = 0
              );
    
    void recycle_bullet(Bullet &bullet);
};
}