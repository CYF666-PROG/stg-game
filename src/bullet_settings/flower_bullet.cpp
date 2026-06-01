#include "flower_bullet.hpp"
#include "../game/bullet/round_bullet.hpp"
#include "../game/bullet_pool.hpp"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/circle_shape2d.hpp>


using namespace game::bullet_settings;

void game::bullet_settings::FlowerBullet::_bind_methods(){}

void FlowerBullet::shoot(){
  double base_angle = get_rotation(); // 获取当前花弹的初始角度

  // 🌟 角度控制参数（可以根据您的需求修改）
  double angle_step = rad; // 🔴 每隔多少弧度发射一次（例如：0.5236 弧度约等于 30 度）
  int angle_count = direction_count;       // 🔴 一共发射多少个方向（例如：12 个方向正好转满 360 度）

  // 计算速度的步长间隔
  double speed_step = 0.0;
  if (count > 1) {
    speed_step = (max - min) / (count - 1);
  }

  // 先行检查游戏场景树，避免在循环中重复检查或报错
  godot::SceneTree* tree = get_tree();
  if (!tree) {
    godot::UtilityFunctions::print("tree error");
    return;
  }
  godot::Node* current_scene = tree->get_current_scene();
  if (!current_scene) {
    godot::UtilityFunctions::print("current_scene error");
    return;
  }

  // 1. 外层循环：控制不同的发射角度
  for (int j = 0; j < angle_count; j++) {
    // 计算当前方向的绝对弧度
    double current_angle = base_angle + (j * angle_step);
    
    // 根据当前弧度计算方向向量
    godot::Vector2 direction(cos(current_angle), sin(current_angle));

    // 2. 内层循环：在当前方向上，生成 count 个速度均匀的子弹
    for (int i = 0; i < count; i++) {
      // 计算当前子弹的均匀速度
      double current_speed = min + (i * speed_step);
      if (count == 1) {
          current_speed = max;
      }

      // 将方向乘以速度，得到最终的物理速度向量
      godot::Vector2 velocity_vector = direction * current_speed;

      auto* pool = game::BulletPool::get_pool();
      if (!pool) return;
      auto* sub_bullet = pool->get_bullet();
      if (!sub_bullet) return;
      auto move = std::make_unique<utility::Move>(velocity_vector);
      move->start_time = 0;
      move->end_time = 1000;
      sub_bullet->moves.push_back(std::move(move));
      sub_bullet->start_position = get_global_position();
      sub_bullet->init(get_global_position());
      // 让场景收养子弹
      godot::UtilityFunctions::print("FlowerBullet::shoot()");
      current_scene->add_child(sub_bullet);
    }
  }
}

void game::bullet_settings::FlowerBullet::frame_do(double delta){
  if (fire_count == 0 || int(fire_interval*60) == 0) return;
  if (frame % int(fire_interval*60) == 0){
    if (!fire_count--) return;
    shoot();
  }
  frame += 1;
}

void game::bullet_settings::FlowerBullet::_ready() {}

FlowerBullet::FlowerBullet(double rad, int direction_count,int count, double max, double min) : rad(rad), direction_count(direction_count),count(count), max(max), min(min) {}
game::bullet_settings::FlowerBullet::FlowerBullet()
{
}
FlowerBullet::~FlowerBullet()
{}