#include "wave.hpp"
#include "godot_cpp/classes/resource_loader.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/vector2.hpp"

using namespace game::bullet_settings::nitori;
using namespace game;
using namespace godot;

void Wave::shoot(){
  if (!is_enabled) return;
  // ==========================================
  // 🌟 弹幕控制参数（全部放在开头，方便修改）
  // ==========================================
  int interval_frames = 30;      // 发射间隔帧数
  double base_speed = 2.0f * double(direction) ;       // 向左推进的基础速度
  double amplitude = 125.0f;        // 🌊 正弦波振幅（上下波动的幅度）
  double frequency = 0.014544;        // 🔄 正弦波频率（数值越大，波浪越密集）

  // ==========================================
  // ⚙️ 帧率计数器控制
  // ==========================================
  frame_counter++;
  if (frame_counter < interval_frames) {
      return; 
  }
  frame_counter = 0; 

  Vector2 spawn_pos = shoot_pos; 
  double pian = fire_rotation_offset; // 保留你原有的旋转偏移

  // ==========================================
  // 🚀 核心：发射两路正弦波子弹
  // ==========================================

  // 基础方向向量：向左飞行 Vector2(-1, 0)
  Vector2 base_direction(-1.0f, 0.0f);

  // --- 1. 第一路子弹（向上波动的正弦波） ---
  auto wave_behavior_up = [base_direction, base_speed, amplitude, frequency, pian](BulletPool::Bullet& b) {
    // 基础水平位移速度
    godot::Vector2 velocity = base_direction * base_speed;
    
    // 计算当前时间点的正弦波纵向速度（对位移求导，或者直接模拟速度）
    // 这里我们直接修改速度向量的 Y 轴：y = A * cos(w * t) * w
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

  // --- 2. 第二路子弹（向下波动的正弦波，通过取反实现一上一下） ---
  auto wave_behavior_down = [base_direction, base_speed, amplitude, frequency, pian](BulletPool::Bullet& b) {
    godot::Vector2 velocity = base_direction * base_speed;
    
    // 注意这里的负号： -amplitude，与上面那路刚好相反，形成完美的交错对称
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
  // 1. 获取资源加载器的单例
  ResourceLoader* loader = ResourceLoader::get_singleton();
  // 2. 直接加载资源并进行安全强转
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