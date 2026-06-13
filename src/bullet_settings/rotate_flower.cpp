#include "rotate_flower.hpp"
#include "../game/bullet_pool.hpp"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/circle_shape2d.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/atlas_texture.hpp>
#include <godot_cpp/classes/sprite2d.hpp>

using namespace game::bullet_settings;
using namespace godot;
using namespace game;

void RotateFlower::shoot(){
  int interval_frames = 8;       // 每隔多少帧发射一次（配合你的计时器使用）
  int way_count = 3;             // 每次发射的方向数（3路平分圆）
  int bullet_per_way = 6;        // 每个方向发射的子弹数量（速度分层）
  
  float rotate_step_deg = 8.0f;  // 每次发射后，基础角度旋转的度数（3度）
  float min_speed = 3.0f;        // 最小速度（像素/帧）
  float max_speed = 6.0f;        // 最大速度（像素/帧）

  frame_counter++;
  if (frame_counter < interval_frames) {
    return;
  }
  frame_counter = 0;

  audio_manager->play("bong00");

  double base_angle_rad = godot::Math::deg_to_rad(current_base_degree);
  // 角度步长：360度 / 方向数，转换为弧度
  double way_step_rad = godot::Math::deg_to_rad(360.0f / way_count);
  // 速度的步长间隔
  double speed_step = 0.0;
  if (bullet_per_way > 1) {
      speed_step = (max_speed - min_speed) / (bullet_per_way - 1);
  }
  shoot_effects();

  // n 个平分圆的方向
  for (int w = 0; w < way_count; w++) {
    // 当前路的绝对弧度 = 基础旋转弧度 + 平分圆的偏移弧度
    double current_way_angle = base_angle_rad + (w * way_step_rad);
    
    // 计算当前方向的向量
    godot::Vector2 direction_vector(cos(current_way_angle), sin(current_way_angle));

    // 在当前方向上，生成速度均匀分布的 6 个子弹
    for (int i = 0; i < bullet_per_way; i++) {
      // 计算当前子弹的速度
      double current_speed = min_speed + (i * speed_step);
      if (bullet_per_way == 1) {
        current_speed = max_speed;
      }

      godot::Vector2 velocity_vector = direction_vector * current_speed;

      auto *new_pool = BulletPool::get_pool();
      if (!new_pool) {
        godot::UtilityFunctions::print("FlowerBullet::shoot_spiral pool not found");
        return;
      }

      Vector2 spawn_pos = get_global_position(); // 发射源位置
      double pian = rotation_offset;              // 子弹贴图旋转偏移
      int jia = 10;                              // 完全加速帧数

      auto linear_behavior = [velocity_vector, pian, jia](BulletPool::Bullet& b){
        b.rotation = pian + b.velocity.angle();
        if (b.lifetime < jia){
          b.velocity = (velocity_vector) * (double(b.lifetime) / double(jia));
        } else {
          b.velocity = velocity_vector;
        }
      };

      new_pool->spawn(
        spawn_pos,     
        linear_behavior,  
        to_launch_texture,  
        "normal",
        6.0f,
        1,
        10,
        zoom
      );
    }
  }

  // 基础角度旋转 3 度
  current_base_degree += rotate_step_deg;
  // 角度保持在 0~360 
  if (current_base_degree >= 360.0f) {
    current_base_degree -= 360.0f;
  }
};