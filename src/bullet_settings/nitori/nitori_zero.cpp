#include "nitori_zero.hpp"

#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/vector2.hpp"
#include "../rotate_flower.hpp"
#include "../../game/bullet_pool.hpp"
#include "../../game/player/player.hpp"
#include "../../conf/bullet.hpp"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/circle_shape2d.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/atlas_texture.hpp>
#include <godot_cpp/classes/sprite2d.hpp>

using namespace game::bullet_settings;
using namespace godot;

void NitoriZero::shoot() {
  fire();
  pointed();
}

NitoriZero::NitoriZero(){
  // 加载子弹
  String path = "res://material/bullet/pointed/blue.tres";
  ResourceLoader* loader = ResourceLoader::get_singleton();
  // 2. 直接加载资源并进行安全强转
  pointed_tex = loader->load(path);
  if (pointed_tex.is_null()){
    UtilityFunctions::print("not fond ",path);
  }
}

void NitoriZero::_ready(){
  game::BulletManager::_ready();
}

void NitoriZero::re_set(){
  // 重置批次
  fire_burst_counter = 0;
  pointed_max_cout = 0;
  // 重置帧计数器
  fire_frame_counter = 0;
  pointed_frame_counter = 0;
}

bool NitoriZero::is_end(){
  if (fire_burst_counter < fire_max_cout) {
    return false;
  }
  if (pointed_burst_counter < pointed_burst_counter) {
    return false;
  }
  return true;
}

void NitoriZero::pointed(){
    double rotation_offset = this->pointed_rotation_offset;
    // ==========================================
    // 🌟 弹幕控制参数
    // ==========================================
    int interval_frames = 10;        // 每隔 10 帧发射一次
    float outward_speed = 3.0f;     // 子弹的速度（每帧 3 像素）
    float arc_radius = 120.0f;      // 半圆半径 150 像素
    
    // 基础配置
    float center_angle_deg = 90.0f; // 正上方
    float angle_step_deg = 10.0f;     // 每隔 6 度发射一颗
    // 做到间隔发射
    pointed_frame_counter++;
    if (pointed_frame_counter < interval_frames) {
        return; 
    }
    pointed_frame_counter = 0; 
    // 大于批次计数直接退出
    if (pointed_burst_counter >= pointed_max_cout) {
      return;
    }
    pointed_burst_counter++;
    // 特效
    audio_manager->fire_bullet();
    shoot_effects();

    godot::Vector2 center_pos = get_global_position(); 
    auto *new_pool = BulletPool::get_pool();
    if (!new_pool) {
        godot::UtilityFunctions::print("BulletPool not found");
        return;
    }

    // 根据速度和半径，计算出圆弧运动的基础通用参数
    double angular_velocity = outward_speed / arc_radius;
    int half_circle_frames = static_cast<int>(Math_PI / angular_velocity);

    // ==========================================
    // 🚀 循环 1：左半边扇面 (总共 90 度：-150° 到 -60°)
    // 轨迹：逆时针绕右侧圆心转动，最终整体折向右上方飞出
    // ==========================================
    float left_start = center_angle_deg - 60.0f; // -150 度
    float left_end   = center_angle_deg + 30.0f; // -60 度

    for (float angle_deg = left_start; angle_deg <= left_end; angle_deg += angle_step_deg) {
        double init_angle_rad = godot::Math::deg_to_rad(angle_deg);
        
        // 圆心在初始方向往右偏 90 度
        double center_offset_angle = init_angle_rad + (Math_PI / 2.0);
        godot::Vector2 circle_center = center_pos + godot::Vector2(cos(center_offset_angle), sin(center_offset_angle)) * arc_radius;
        double initial_angle_to_center = (circle_center - center_pos).angle() + Math_PI;

        auto left_behavior = [circle_center, arc_radius, initial_angle_to_center, angular_velocity, half_circle_frames, outward_speed, rotation_offset](BulletPool::Bullet& b) {
            if (b.lifetime <= half_circle_frames) {
                // 逆时针旋转 (-1.0)
                double current_arc_angle = initial_angle_to_center + (angular_velocity * b.lifetime * -1.0);
                godot::Vector2 target_pos = circle_center + godot::Vector2(cos(current_arc_angle), sin(current_arc_angle)) * arc_radius;
                b.velocity = target_pos - b.position;
            } else {
                double final_arc_angle = initial_angle_to_center + (angular_velocity * half_circle_frames * -1.0);
                double tangent_angle = final_arc_angle + (-1.0 * Math_PI / 2.0);
                b.velocity = godot::Vector2(cos(tangent_angle), sin(tangent_angle)) * outward_speed;
            }
            if (b.velocity != godot::Vector2(0, 0)) b.rotation = rotation_offset + b.velocity.angle();
        };

        new_pool->spawn(center_pos, left_behavior, pointed_tex, "normal", pointed_radius, 1, 10, Vector2(1,1) * pointed_scale);
    }

    // ==========================================
    // 🚀 循环 2：右半边扇面 (总共 90 度：-120° 到 -30°)
    // 轨迹：顺时针绕左侧圆心转动，最终整体折向左上方飞出
    // ==========================================
    float right_start = center_angle_deg - 30.0f; // -120 度
    float right_end   = center_angle_deg + 60.0f; // -30 度

    for (float angle_deg = right_start; angle_deg <= right_end; angle_deg += angle_step_deg) {
        double init_angle_rad = godot::Math::deg_to_rad(angle_deg);
        
        // 圆心在初始方向往左偏 90 度
        double center_offset_angle = init_angle_rad - (Math_PI / 2.0);
        godot::Vector2 circle_center = center_pos + godot::Vector2(cos(center_offset_angle), sin(center_offset_angle)) * arc_radius;
        double initial_angle_to_center = (circle_center - center_pos).angle() + Math_PI;

        auto right_behavior = [circle_center, arc_radius, initial_angle_to_center, angular_velocity, half_circle_frames, outward_speed, rotation_offset](BulletPool::Bullet& b) {
            if (b.lifetime <= half_circle_frames) {
                // 顺时针旋转 (1.0)
                double current_arc_angle = initial_angle_to_center + (angular_velocity * b.lifetime * 1.0);
                godot::Vector2 target_pos = circle_center + godot::Vector2(cos(current_arc_angle), sin(current_arc_angle)) * arc_radius;
                b.velocity = target_pos - b.position;
            } else {
                double final_arc_angle = initial_angle_to_center + (angular_velocity * half_circle_frames * 1.0);
                double tangent_angle = final_arc_angle + (1.0 * Math_PI / 2.0);
                b.velocity = godot::Vector2(cos(tangent_angle), sin(tangent_angle)) * outward_speed;
            }
            if (b.velocity != godot::Vector2(0, 0)) b.rotation = rotation_offset + b.velocity.angle();
        };

        new_pool->spawn(center_pos, right_behavior, pointed_tex, "normal", pointed_radius, 1, 10, Vector2(1,1) * pointed_scale);
    }
}

void NitoriZero::fire(){
  // 加载炎弹
  // 1. 获取资源加载器的单例
  ResourceLoader* loader = ResourceLoader::get_singleton();
  // 2. 直接加载资源并进行安全强转
  Ref<SpriteFrames> spr = loader->load("res://material/bullet/fire/blue.tres");
  if (spr.is_null()){
    UtilityFunctions::print("not fond ","res://material/bullet/fire/blue.tres");
  }
  // 贴图旋转偏移
  double rotation_offset = conf::bullet::fire::rotation_offset;
  // ==========================================
  // 🌟 弹幕控制参数（五大核心参数，完全解耦）
  // ==========================================
  int interval_frames = 5;           // 连发时每次开火的间隔帧数
  int way_count = 18;                 // 18 个圆周方向
  int bullet_per_way = 2;             // 每个方向发射 2 颗子弹（一顺一逆偏移）
  
  float outward_speed = 6.0f;         // 子弹运动时的直线扩散速度（像素/帧）
  float rotate_step_deg = 2.0f;       // 每次发射后，基础角度递增的步长

  // 🎛️ 时序核心控制参数
  int burst_count = 20;                // 一次大循环里连续开火几次（批次数）
  int stop_delay_frames = 18;         // 子弹发射后，先向前正常飞行多少帧再急刹车
  int stop_duration_base = 45;        // 第一批子弹原地停顿的基础帧数
  int activation_interval_frames = 15;// 后续批次破封起跑的依次间隔帧数

  // ==========================================
  // ⏳ ⭐ 核心修改：单次循环拦截器
  // ==========================================

  
  // 如果已经发射完成了指定的批次数，直接返回，彻底停止后续的所有发射
  if (fire_burst_counter >= burst_count) {
      return; 
  }

  fire_frame_counter++;
  if (fire_frame_counter < interval_frames) {
      return; 
  }
  fire_frame_counter = 0;

  // 锁定当前批次的索引（0 到 burst_count - 1）
  int this_bullet_batch = fire_burst_counter; 
  fire_burst_counter++; // 批次递增

  // ==========================================
  // 📐 初始化发射参数
  // ==========================================
  double current_offset_deg = this->fire_current_base_degree;
  double way_step_rad = godot::Math::deg_to_rad(360.0f / way_count);
  
  shoot_effects();
  godot::Vector2 center_pos = get_global_position(); 

  // ==========================================
  // 🚀 弹幕发射双层循环
  // ==========================================
  for (int w = 0; w < way_count; w++) {
      double way_base_angle_deg = (w * (360.0f / way_count));

      for (int i = 0; i < bullet_per_way; i++) {
          double offset_dir = (i == 0) ? 1.0 : -1.0;

          double final_bullet_deg = way_base_angle_deg + (current_offset_deg * offset_dir);
          double final_bullet_angle_rad = godot::Math::deg_to_rad(final_bullet_deg);

          godot::Vector2 velocity_vector(cos(final_bullet_angle_rad), sin(final_bullet_angle_rad));
          velocity_vector *= outward_speed;

          auto *new_pool = BulletPool::get_pool();
          if (!new_pool) {
              godot::UtilityFunctions::print("BulletPool not found");
              return;
          }

          // 🛠️ 终极时序矩阵 Lambda
          auto timed_matrix_behavior = [
              velocity_vector, 
              rotation_offset, 
              stop_delay_frames, 
              stop_duration_base,
              activation_interval_frames,
              interval_frames,
              this_bullet_batch       
          ](BulletPool::Bullet& b) {
              
              // ⏳ 阶段 1：初生扩散期
              if (b.lifetime <= stop_delay_frames) {
                  b.velocity = velocity_vector;
              }
              // ⏳ 阶段 2 & 3：精准时序破封
              else {
                  int wake_up_lifetime = stop_delay_frames + stop_duration_base + 
                                          (this_bullet_batch * (activation_interval_frames - interval_frames));

                  if (b.lifetime < wake_up_lifetime) {
                      b.velocity = godot::Vector2(0, 0); 
                  } else {
                      b.velocity = velocity_vector; 
                  }
              }

              // 🔄 贴图朝向更新
              if (b.velocity != godot::Vector2(0, 0)) {
                  b.rotation = rotation_offset + b.velocity.angle();
              } else {
                  b.rotation = rotation_offset + velocity_vector.angle();
              }
          };

          // 压入弹幕池
          new_pool->spawn(
              center_pos,     
              timed_matrix_behavior, 
              spr,  
              "normal",
              conf::bullet::fire::radius,
              1,
              10,
              Vector2(1,1)*conf::bullet::fire::scale
          );
      }
  }

  // ==========================================
  // 🔄 更新下一次发射的基础偏移角度
  // ==========================================
  this->fire_current_base_degree += rotate_step_deg;

  if (this->fire_current_base_degree >= 360.0) {
      this->fire_current_base_degree -= 360.0;
  }
};