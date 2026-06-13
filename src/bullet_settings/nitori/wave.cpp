#include "wave.hpp"
#include "godot_cpp/classes/resource_loader.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/vector2.hpp"

using namespace game::bullet_settings::nitori;
using namespace game;
using namespace godot;

void Wave::shoot(){
  if (!is_enabled) return;

  int interval_frames = 30;      // 发射间隔帧数
  double base_speed = 2.0f * double(direction) ;       // 向左推进的基础速度
  double amplitude = 125.0f;        // 振幅
  double frequency = 0.014544;        // 频率

  frame_counter++;
  if (frame_counter < interval_frames) {
      return; 
  }
  frame_counter = 0; 

  Vector2 spawn_pos = shoot_pos; 
  double pian = fire_rotation_offset; // 旋转偏移

  // 基础方向向量 向左飞行
  Vector2 base_direction(-1.0f, 0.0f);

  // 第一路 向上
  auto wave_behavior_up = [base_direction, base_speed, amplitude, frequency, pian](BulletPool::Bullet& b) {
    // 基础水平位移速度
    godot::Vector2 velocity = base_direction * base_speed;
    
    // 修改速度向量的 Y 轴：y = A * cos(w * t) * w
    float cos_wave = amplitude * frequency * cos(frequency * b.lifetime);
    velocity.y += cos_wave;

    b.velocity = velocity;
    b.rotation = pian + b.velocity.angle();
  };

  pool->spawn(
    spawn_pos, 
    wave_behavior_up, 
    fire_tex, 
    "normal", fire_radius, 1, 10, Vector2(1,1)*fire_scale
  );

  // 第二路 向下
  auto wave_behavior_down = [base_direction, base_speed, amplitude, frequency, pian](BulletPool::Bullet& b) {
    godot::Vector2 velocity = base_direction * base_speed;
    
    // -amplitude，与上一路相反
    float cos_wave = -amplitude * frequency * cos(frequency * b.lifetime);
    velocity.y += cos_wave;

    b.velocity = velocity;
    b.rotation = pian + b.velocity.angle();
  };
  pool->spawn(
    spawn_pos, 
    wave_behavior_down, 
    fire_tex, 
    "normal", fire_radius, 1, 10, Vector2(1,1)*fire_scale
  );
};

void Wave::re_set(){
  frame_counter = 0;
  shoot_pos = Vector2(1,1);
  direction = 1;
  is_enabled = false;
}

Wave::Wave(){
  // 加载炎弹
  ResourceLoader* loader = ResourceLoader::get_singleton();
  fire_tex = loader->load("res://material/bullet/fire/blue.tres");
  if (fire_tex.is_null()){
    UtilityFunctions::print("not fond ","res://material/bullet/fire/blue.tres");
  }
  // 获取弹幕池
  pool = BulletPool::get_pool();
  if (!pool) {
    godot::UtilityFunctions::print("Wave::Wave() pool not found");
  }
};