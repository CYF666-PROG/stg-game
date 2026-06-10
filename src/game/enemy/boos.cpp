#include "boos.hpp"
#include "../../utility/clear.hpp"
#include "../bullet_manager.hpp"

#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/vector2.hpp"
#include <godot_cpp/classes/scene_tree.hpp>

using namespace game;
using namespace godot;

bool Boos::move(Vector2 p_target){
  godot::Vector2 current_pos = get_global_position();
  // -------------------------------------------------------------
  // ⚙️ 纯帧硬核物理参数配置（单位皆为：像素/帧）
  // -------------------------------------------------------------
  float max_speed = 6.0f;       // 最高巡航速度（每帧最多走 6 像素）
  float accel = 0.2f;           // 每帧的加速度（数值越小，启动越平滑越慢）
  float brake_distance = 30.0f; // 刹车距离（离终点还剩 90 像素时开始减速缓动）

  // 1. 计算当前位置指向终点的绝对方向和总剩余距离
  godot::Vector2 to_target = p_target - current_pos;
  float distance_left = to_target.length();

  // 安全检查：如果已经贴到目标点了，直接强行靠岸
  if (distance_left <= 2) {
    set_global_position(p_target);
    current_velocity = godot::Vector2(0, 0); // 速度清零
    return true;
  }

  godot::Vector2 move_dir = to_target.normalized();

  // -------------------------------------------------------------
  // 🌀 2. 动态速度计算（核心算法）
  // -------------------------------------------------------------
  float target_speed = max_speed;

  // 🛑 减速区判断：如果进入了刹车距离
  if (distance_left < brake_distance) {
    // 计算减速因子 (从 1.0 渐变到 0.0)
    float brake_factor = distance_left / brake_distance;
    
    // 【高级视觉优化】使用二次方缓动曲线，让减速动作“先快后慢”，靠岸更优雅
    brake_factor = brake_factor * brake_factor; 
    
    target_speed = max_speed * brake_factor;
  }

  // 🚀 加速与平滑过渡：让当前速度向目标速度匀速逼近
  // 使用 move_toward 的标量版本（或者手动逼近），防止速度突变导致的画面抖动
  float current_speed_length = current_velocity.length();
  float next_speed_length = godot::Math::move_toward(current_speed_length, target_speed, accel);

  // 重新组合成最新的速度向量
  current_velocity = move_dir * next_speed_length;

  // -------------------------------------------------------------
  // 🚚 3. 应用位移与绝对靠岸保护
  // -------------------------------------------------------------
  // 如果这一帧走完就会冲过头，或者剩余距离已经极其微小，直接拉到终点
  if (current_velocity.length() >= distance_left) {
    set_global_position(p_target);
    current_velocity = godot::Vector2(0, 0); // 刹车稳住
    
    // 🛑 移动完成，平滑切换状态
    return true;
  } 
  else {
    // 正常物理推进
    set_global_position(current_pos + current_velocity);
  }
  return false;
}

void Boos::next(){
  clear(); // 清楚残留弹幕和残留的发射器
  invincible_frame = 100;
  if (level == 0) {
    start_1();
  }else if (level == 1) {
    start_2();
  }else if (level == 2) {
    start_3();
  }else if (level == 3) {
    start_4();
  }else if (level == 4) {
    start_5();
  }else if (level == 5) {
    start_6();
  }else if (level == 6) {
    dead();
  }
}

void Boos::clear(){
  // 全屏消弹
  auto clear = memnew(utility::Clear);
  get_tree()->get_current_scene()->add_child(clear);
  clear->set_global_position(get_global_position());
  // 清楚残留发射器
  auto arr = get_children();
  for (auto it : arr) {
    auto a = Object::cast_to<BulletManager>(it);
    if (a) {
      a->queue_free();
    }
  }
}

void Boos::dead(){
  queue_free();
}

void Boos::hit_bullet(){
  if(invincible_frame > 0) return;
  hp -= 1;
}

void Boos::entity_physics_process(double delta){
  if (hp <= 0) next();

  // 方便lamb知道自己这个动作持续了多少帧
  if (befor_status != status) {
    frame_status = 0;
  }
  befor_status = status;
  if (status == moveing && move_l) {
    move_l(this);
  }else if (status == shooting && shoot_l) {
    shoot_l(this);
  }
  frame_status++;
  if(invincible_frame > 0) invincible_frame--;
}

void Boos::_ready(){
  // 调用父类的准备
  Enemy::_ready();
  set_global_position(Vector2(141,-118));
  auto target_pos = Vector2(478,202);
  move_l = [target_pos](Boos* b){
    if(b->move(target_pos)){
      b->start_0();
      b->status = shooting;
      b->level = 0;
    };
  };
}