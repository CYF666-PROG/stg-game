#include "three.hpp"
#include "godot_cpp/classes/resource_loader.hpp"
#include "godot_cpp/variant/vector2.hpp"

#include <godot_cpp/variant/utility_functions.hpp>
#include <cmath>

using namespace game::bullet_settings::nitori;
using namespace godot;

Three::Three() {
  frame_counter = 0;
  time_accumulator = 0.0;
  // 加载炎弹
  // 1. 获取资源加载器的单例
  ResourceLoader* loader = ResourceLoader::get_singleton();
  // 2. 直接加载资源并进行安全强转
  fire_tex = loader->load("res://material/bullet/fire/blue.tres");
  if (fire_tex.is_null()){
    UtilityFunctions::print("not fond ","res://material/bullet/fire/blue.tres");
  }
}

void Three::shoot() {
  // 每一帧累加时间，用于让第一阶段的发 shè 角度产生平滑的摆动
  time_accumulator += 0.016; 

  frame_counter++;
  if (frame_counter < interval_frames) {
      return; 
  }
  frame_counter = 0;

  auto *pool = BulletPool::get_pool();
  if (!pool) {
      UtilityFunctions::print("Three::shoot pool not found");
      return;
  }

  // ==========================================
  // 📍 1. 参数化范围：屏幕顶部 y=0 随机生成位置
  // ==========================================
  float random_x = UtilityFunctions::randf_range(spawn_min_x, spawn_max_x);
  Vector2 spawn_pos(random_x, 0.0f);

  // ==========================================
  // 📐 2. 角度计算（随时间左右摆动）与目标点随机
  // ==========================================
  // 第一阶段角度随时间摆动
  float current_swing_deg = stage1_base_deg + std::sin(time_accumulator * stage1_swing_speed) * stage1_swing_amp;
  double launch_rad = Math::deg_to_rad(current_swing_deg);
  
  // 计算方向向量（两阶段相同，因此后续直接沿用此方向）
  Vector2 move_direction = Vector2(std::cos(launch_rad), std::sin(launch_rad)).normalized();

  // 每一颗子弹独一无二的停顿 Y 坐标（234 ± 30 范围内）
  float bullet_target_y = target_y_base + UtilityFunctions::randf_range(-target_y_range, target_y_range);

  // ==========================================
  // ⚙️ 3. 核心：带状态机的 Lambda 运动控制闭包
  // ========================================== 
  double bullet_max_speed = this->bullet_max_speed;
  double fire_rotation_offset = this->fire_rotation_offset;

  auto stop_and_go_behavior = [
    fire_rotation_offset, 
    move_direction,
    bullet_max_speed,
    bullet_target_y,
    stop_duration = this->stop_duration,
    stage = 0,                 // 🌟 状态机：0-阶段1飞行，1-停顿中，2-阶段2飞行
    stop_start_time = 0.0
  ](BulletPool::Bullet& b) mutable {

    // --- 状态 0：阶段 1 斜向下飞行 ---
    if (stage == 0) {
      float distance_to_target = bullet_target_y - b.position.y;
      
      if (distance_to_target < 40.0f) {
        // 接近目标，平滑刹车
        float t = distance_to_target / 40.0f;
        if (t < 0.0f) t = 0.0f; // 防止越界负数
        b.velocity = move_direction * (bullet_max_speed * t);
      } else {
        b.velocity = move_direction * bullet_max_speed;
      }

      // 到达或超越目标 Y，切入停顿状态
      if (b.position.y >= bullet_target_y-2) { // 这里需要减1 不然他永远只是靠近到达不了
        b.position.y = bullet_target_y; // 位置微调修正
        b.velocity = Vector2(0, 0);
        stage = 1;                       // 🌟 切换到状态 1
        stop_start_time = b.lifetime;    // 记录停顿开始时间
      }
    }
    // --- 状态 1：停顿期 ---
    else if (stage == 1) {
      b.velocity = Vector2(0, 0); // 锁死速度

      // 停顿时间到，切入阶段 2
      if ((b.lifetime - stop_start_time) >= stop_duration) {
        stage = 2;               // 🌟 切换到状态 2
      }
    }
    // --- 状态 2：再次斜向下出发 ---
    else if (stage == 2) {
      double time_since_resume = (b.lifetime - stop_start_time) - stop_duration;
      
      if (time_since_resume < 0.2) {
        // 0.2 秒内推背加速
        float t = time_since_resume / 0.2;
        b.velocity = move_direction * (bullet_max_speed * t);
      } else {
        b.velocity = move_direction * bullet_max_speed;
      }
    }

    // 刷新子弹贴图朝向
    if (b.velocity.length_squared() > 0.001f) {
      b.rotation = b.velocity.angle() + fire_rotation_offset;
    }
  };
  // 音效
  audio_manager->play("fast");
  pool->spawn(
    spawn_pos, 
    stop_and_go_behavior, 
    fire_tex, 
    "normal",
    fire_radius,
    1,
    10,
    Vector2(1,1) * fire_scale
  );
}