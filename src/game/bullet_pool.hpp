#pragma once

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/variant/rid.hpp>
#include <godot_cpp/variant/rect2.hpp>


#include <vector>
#include <functional> // 引入现代 C++ 标准函数库

namespace game{

struct BulletConfig {
  godot::Ref<godot::Texture2D> texture;
  int h_frames = 1;
  int v_frames = 1;
  float radius = 4.0f;
  float anim_speed = 10.0f;
  bool is_atlas = false;
  /// @brief 矩形 包含左上角坐标，长宽
  godot::Rect2 atlas_region = godot::Rect2();
};

class BulletPool : public godot::Node2D {
  GDCLASS(BulletPool, Node2D)
public:
  struct Bullet {
    godot::RID area_rid;
    godot::RID shape_rid;
    godot::RID canvas_item_rid;
    
    godot::Vector2 position;
    /// @brief 速度
    godot::Vector2 velocity;
    /// @brief 初始速度
    godot::Vector2 base_velocity; // 保存初始速度，方便曲线运动计算
    /// @brief 旋转角
    float rotation = 0.0f;
    /// @brief 子弹已存活的帧数
    int lifetime = 0.0f; 
    bool active = false;

    BulletConfig config;
    float anim_timer = 0.0f;
    int current_frame = 0;

    // 【核心修改】现代 C++ 函数指针：接收当前子弹引用和 delta，返回新速度
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
  
  // 【核心修改】spawn 接口中的 p_vel 改为传入 std::function
    void spawn(godot::Vector2 p_pos, 
               std::function<void(Bullet&)> p_behavior, 
               float p_rot, 
               godot::Ref<godot::Texture2D> p_tex, int p_h_frames, int p_v_frames, 
               float p_radius, float p_anim_speed);
    
    void recycle_bullet(Bullet &bullet);
};
}