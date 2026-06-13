#include "strafe.hpp"

#include "godot_cpp/variant/vector2.hpp"
#include "rotate_flower.hpp"
#include "../game/bullet_pool.hpp"
#include "../game/player/player.hpp"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/circle_shape2d.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/atlas_texture.hpp>
#include <godot_cpp/classes/sprite2d.hpp>

using namespace game::bullet_settings;
using namespace godot;
using namespace game;

void Strafe::shoot(){
  if (!fire_count) {
    return; 
  }
  auto player = get_node<game::Player>("/root/play/Player");
  if(!player) {
    UtilityFunctions::print("Strafe::shoot player not foud");
    return;
  }

  int interval_frames = 4;       // 每隔多少帧发射一次
  double target_deg = 90.0f;      // 目标中心方向
  double rotate_step_deg = 6.0f;  // 每次发射后偏转的度数
  double bullet_speed = 5.0f;     // 子弹速度
  
  bool ping_pong = true;         // true 为来回往复扫射，false 为单向循环扫射
  // 音效
  audio_manager->play("fast");

  frame_counter++;
  if (frame_counter < interval_frames) {
    return;
  }
  frame_counter = 0; 
  // 音效
  shoot_effects();
  // 绝对发射角度 = 目标中心方向 + 当前偏转位移
  double absolute_deg = target_deg + current_offset_deg;
  double launch_angle_rad = godot::Math::deg_to_rad(absolute_deg);
  // 计算方向向量
  godot::Vector2 direction_vector(cos(launch_angle_rad), sin(launch_angle_rad));
  godot::Vector2 velocity_vector = direction_vector * bullet_speed;

  auto *new_pool = BulletPool::get_pool();
  if (!new_pool) {
    godot::UtilityFunctions::print("RotateFlower::shoot_sweep pool not found");
    return;
  }

  Vector2 spawn_pos = get_global_position(); 
  double pian = rotation_offset;              
  double jia = 10;                              

  auto linear_behavior = [velocity_vector, pian, jia](BulletPool::Bullet& b){
    b.rotation = pian + b.velocity.angle();
    b.velocity = velocity_vector + velocity_vector.normalized()*(30.0/(b.lifetime*1.2+1));
  };

  new_pool->spawn(
    spawn_pos,     
    linear_behavior,  
    to_launch_texture,  
    "normal",
    radius,
    1,
    10,
    zoom
  );
  // 推进扫射进度
  // 按照步长和当前方向移动
  current_offset_deg += rotate_step_deg * sweep_direction;

  double half_range = sweep_range_deg / 2.0f;

  if (ping_pong) {
    // 往复模式 到头反弹：-30° -> +30° -> -30°
    if (current_offset_deg >= half_range) {
      current_offset_deg = half_range;
      sweep_direction = -1.0f; // 调头向左扫
      fire_count--;
    } else if (current_offset_deg <= -half_range) {
      current_offset_deg = -half_range;
      sweep_direction = 1.0f;  // 调头向右扫
      fire_count--;
    }
  } else {
    // 单向模式 到头瞬间回到最左边：-30° -> +30°
    if (current_offset_deg > half_range) {
      current_offset_deg = -half_range;
      fire_count--;
    }
  }
};

Strafe::Strafe(){
  current_offset_deg = sweep_direction * sweep_range_deg / 2.0;
}