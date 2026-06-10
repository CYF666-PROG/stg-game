#include "four.hpp"
#include "godot_cpp/classes/resource_loader.hpp"
#include "godot_cpp/core/math.hpp"
#include "godot_cpp/variant/vector2.hpp"

using namespace game::bullet_settings::nitori;
using namespace godot;

Four::Four(){
  // 加载炎弹
  // 1. 获取资源加载器的单例
  ResourceLoader* loader = ResourceLoader::get_singleton();
  // 加载炎弹
  ellipse_tex = loader->load("res://material/bullet/ellipse/green.tres");
  if (ellipse_tex.is_null()){
    UtilityFunctions::print("not fond ","res://material/bullet/ellipse/green.tres");
  }
  // 加载米弹
  rice_tex = loader->load("res://material/bullet/rice/cyan.tres");
  if (ellipse_tex.is_null()){
    UtilityFunctions::print("not fond ","res://material/bullet/rice/cyan.tres");
  }
};

bool Four::is_end(){
  if(ring_frame_count > 25) return true;
  return false;
}

void Four::re_set(){
  ring_frame_count = 0;
}

void Four::run_to_shoot_player(){run_shoot_player = true;}

void Four::shoot(){
  shoot_player();
  shoot_rings();
  ring_frame_count++;
}

void Four::disabled(){
  ring_frame_count = 100;
}

void Four::shoot_player(){
  if (!run_shoot_player) return;
  // 每几帧发射
  if (frame % 5 != 0) return;
  double max_offset_deg = 8;    // 🌟 最大随机偏移角度（例如 10.0f，表示在玩家方向左右 ±10度内晃动）
  double bullet_speed = 2;       // 子弹发射的速度

  // 2. 🎯 动态计算指向玩家的绝对角度
  double to_player_angle = get_global_position().angle_to_point(player->get_global_position());

  // 3. 注入随机偏角，得到最终的子弹发射角度
  float rand_offset_deg = UtilityFunctions::randf_range(-max_offset_deg, max_offset_deg);
  double final_angle = to_player_angle + godot::Math::deg_to_rad(rand_offset_deg);

  // 4. 根据角度和速度直接计算速度向量
  godot::Vector2 velocity_vector(cos(final_angle), sin(final_angle));
  velocity_vector *= bullet_speed;

  Vector2 spawn_pos = get_global_position(); // 当前发射源位置
  int jia = 10;                              // 完全加速帧数（推背感）

  // 6. 绑定线性推背加速行为
  auto linear_behavior = [
    velocity_vector, 
    jia
  ](BulletPool::Bullet& b) {
      if (b.lifetime < jia) {
          b.velocity = velocity_vector * (double(b.lifetime) / double(jia));
      } else {
          b.velocity = velocity_vector;
      }
  };

  // 7. 直接压入弹幕池（没有循环，只生一颗）
  pool->spawn(
      spawn_pos,
      linear_behavior,
      rice_tex,  
      "normal",
      rice_radius,
      1,
      10,
      Vector2(1,1) * rice_scale
  );
}

void Four::shoot_rings() {
  // 基础参数本地化引用
  int b_count = this->b_count;              // 假设为 30
  float base_radius = this->target_radius;  // 基础停顿半径
  float base_speed2 = this->stage2_max_speed; // 基础扩散速度
  int base_stop_duration = this->stop_duration_frames; // 基础停顿时间（基准值）

  // 计算单个子弹间距的“半缝隙弧度”（用于同组内完美插缝）
  double half_gap_rad = Math_PI / (double)b_count; 

  // =================================================================
  // ⏱️ 时间轴核心逻辑：每组环间隔 10 帧发出，动态扣减蓄力时间以实现同时爆发
  // =================================================================
  
  switch (ring_frame_count) {
    // ------ 🔹 第一组：内圈组合（第 5 帧发射） ------
    case 25:
      // 环 1-1：正常发射，基准蓄力时间
      shoot_ring(0.0, base_radius, base_speed2, base_stop_duration);
      // 环 1-2：同组内同时发射并完美插缝，速度较快
      shoot_ring(half_gap_rad, base_radius, base_speed2 + 1.0f, base_stop_duration);
      break;
        
    // ------ 🔹 第二组：中圈组合（第 15 帧发射，间隔 10 帧） ------
    case 15: 
      {
        double group2_offset = Math::deg_to_rad(15.0); 
        float radius_g2 = base_radius + 40.0f;
        // 🌟 核心：因为晚发了 10 帧，所以蓄力时间必须缩短 10 帧
        int g2_stop_duration = base_stop_duration - 10;

        // 环 2-1
        shoot_ring(group2_offset, radius_g2, base_speed2 + 0.2f, g2_stop_duration);
        // 环 2-2：插缝
        shoot_ring(group2_offset + half_gap_rad, radius_g2, base_speed2 + 1.2f, g2_stop_duration);
      }
      break;

    // ------ 🔹 第三组：外圈组合（第 25 帧发射，再间隔 10 帧） ------
    case 5: 
      {
        double group3_offset = Math::deg_to_rad(30.0);
        float radius_g3 = base_radius + 80.0f;
        // 🌟 核心：因为比第一组晚发了 20 帧，所以蓄力时间必须缩短 20 帧
        int g3_stop_duration = base_stop_duration - 20;

        // 环 3-1
        shoot_ring(group3_offset, radius_g3, base_speed2 + 0.4f, g3_stop_duration);
        // 环 3-2：插缝
        shoot_ring(group3_offset + half_gap_rad, radius_g3, base_speed2 + 1.4f, g3_stop_duration);
      }
      break;

    default:
      // 其他帧直接放行
      break;
  }
}

void Four::shoot_ring(
  double base_offset_rad, // 初始偏移弧度
  double target_radius, // 停下时的半径
  double stage2_max_speed, // 停顿后扩散的速度
  int stop_duration_frames  // 停顿的帧数
){  
  // 发射中心点
  Vector2 center_pos = get_global_position();

  // 基础参数本地化引用
  int b_count = this->b_count;
  float orbit_linear_speed = this->orbit_linear_speed; 
  double ellipse_rotation_offset = this->ellipse_rotation_offset;

  // 分流独立速度与加速帧数
  float stage1_max_speed = this->stage1_max_speed;
  float stage2_accel_frames = 300; // 你代码中的 300 帧

  // 🌟 将初始偏移角度从“角度制”转换为“弧度制”

  // 一次性发射 b_count 颗，均匀分布在 360 度
  for (int i = 0; i < b_count; ++i) {
    double fraction = (double)i / (double)b_count;
    
    // 🌟 核心改动：在原先均匀分布的角度上，加上整体的基础偏移角
    double initial_angle_rad = (fraction * Math_PI * 2.0) + base_offset_rad; 

    auto orbit_behavior = [
      center_pos,
      initial_angle_rad,
      stage1_max_speed,      
      stage2_max_speed,      
      stage2_accel_frames,   
      target_radius,
      stop_duration_frames,
      orbit_linear_speed, 
      ellipse_rotation_offset,
      stage = 0,               
      current_radius = 0.0f,   
      current_angle = initial_angle_rad, // 会直接继承带偏移的初始角度
      stop_start_frame = 0,
      rotation_speed_rad = Math::deg_to_rad(rotation_speed_deg)
    ](BulletPool::Bullet& b) mutable {

      // --- 状态 0：从中心向外发射，使用 stage1_max_speed 平滑减速靠岸 ---
      if (stage == 0) {
        current_radius = center_pos.distance_to(b.position);
        float distance_left = target_radius - current_radius;

        if (distance_left <= 1.0f) {
          current_radius = target_radius;
          b.velocity = Vector2(0, 0);
          stage = 1;
          stop_start_frame = b.lifetime; 
        } 
        else if (distance_left < 30.0f) {
          float t = distance_left / 30.0f;
          Vector2 dir = Vector2(std::cos(current_angle), std::sin(current_angle));
          b.velocity = dir * (stage1_max_speed * t);
        } 
        else {
          Vector2 dir = Vector2(std::cos(current_angle), std::sin(current_angle));
          b.velocity = dir * stage1_max_speed;
        }
      }
      // --- 状态 1：在圆周上停顿蓄力（保持静止不转，等待爆发） ---
      else if (stage == 1) {
        b.velocity = Vector2(0, 0); 

        // 锁死刷新极坐标位置
        b.position = center_pos + Vector2(std::cos(current_angle), std::sin(current_angle)) * current_radius;

        if ((b.lifetime - stop_start_frame) >= stop_duration_frames) {
            stage = 2;
        }
      }
      // --- 状态 2：解除封印，向外扩散（扩散与公转双重加速） ---
      else if (stage == 2) {
        int frames_since_resume = b.lifetime - stop_start_frame - stop_duration_frames;
        
        float t = 1.0f;
        if (frames_since_resume < stage2_accel_frames) {
            t = (float)frames_since_resume / stage2_accel_frames;
        }
        
        float current_spread_speed = stage2_max_speed * t;
        current_radius += current_spread_speed;

        float current_orbit_linear_speed = orbit_linear_speed * t;
        
        double current_frame_omega = current_orbit_linear_speed / current_radius;
        current_angle += current_frame_omega;

        b.position = center_pos + Vector2(cos(current_angle), sin(current_angle)) * current_radius;
        
        Vector2 out_dir = Vector2(cos(current_angle), sin(current_angle));
        Vector2 tangent_dir = Vector2(-sin(current_angle), cos(current_angle)); 
        
        b.velocity = (out_dir * current_spread_speed) + (tangent_dir * current_orbit_linear_speed);
      }

      // --- 刷新子弹自转与贴图朝向 ---
      if (b.lifetime == 0) {
        b.rotation = b.velocity.angle() + ellipse_rotation_offset;
      } else {
        b.rotation += rotation_speed_rad;
      }
    };

    // ==========================================
    // 🚀 压入对象池
    // ==========================================
    pool->spawn(
        center_pos, 
        orbit_behavior, 
        ellipse_tex, 
        "normal",
        ellipse_radius,
        1,
        10,
        Vector2(1,1) * ellipse_scale
    );
  }
}