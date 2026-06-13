#include "flower_bullet.hpp"
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
using namespace game ;


void FlowerBullet::shoot(){
  auto player = get_node<game::Player>("/root/play/Player");
  if(!player) return;
  shoot_effects();

  // 初始角度
  double base_angle = 0;
  // 大于185则跟踪
  if (launch_angle >= 185) {
    base_angle = get_global_position().angle_to_point(player->get_global_position());
  }else {
    base_angle = godot::Math::deg_to_rad(launch_angle);
  }

  // 角度控制参数
  double angle_step = godot::Math::deg_to_rad(rad);            // 每隔多少弧度发射一次
  int angle_count = direction_count;  // 一共发射多少个方向

  // 计算速度的步长间隔
  double speed_step = 0.0;
  if (count > 1) {
      speed_step = (max - min) / (count - 1);
  }
  // 音效
  if (direction_count == 1 && count == 1) {
    audio_manager->play("fast");
  }else {
    audio_manager->play("bong00");
  }

  // 控制不同的发射角度
  for (int j = 0; j < angle_count; j++) {
    // 索引 j 相对于中心点的偏移系数
    // 奇数时 j = 0 得到 -(count-1)/2，对应最左边 中间项为 0
    // 偶数时 j = 0 得到 -count/2 + 0.5，两侧对称，中心留空
    double angle_offset = (j - (angle_count - 1) / 2.0) * angle_step;
    // 计算当前方向的绝对弧度
    double current_angle = base_angle + angle_offset;
    // 根据当前弧度计算方向向量
    godot::Vector2 direction_vector(cos(current_angle), sin(current_angle));
    // 在当前方向上，生成 count 个速度均匀分布的子弹
    for (int i = 0; i < count; i++) {
      // 计算当前子弹的均匀速度
      double current_speed = min + (i * speed_step);
      if (count == 1) {
        current_speed = max;
      }
      // 将方向乘以速度，得到物理速度向量
      godot::Vector2 velocity_vector = direction_vector * current_speed;
      auto *new_pool = BulletPool::get_pool();
      if (!new_pool) {
        UtilityFunctions::print("FlowerBullet::shoot pool not found");
        return;
      }
      Vector2 spawn_pos = get_global_position(); // 发射源当前位置
      
      float pian = 1.5; // 子弹贴图旋转偏移
      int jia = 10;     // 完全加速帧数
      
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
        radius,
        1,
        10,
        zoom
      );
    }
  }
}

void game::bullet_settings::FlowerBullet::frame_do(double delta){
  if (fire_count == 0 || int(fire_interval) == 0) return;
  if (frame % fire_interval == 0){
    if (!fire_count--) {
      queue_free(); // 发射完毕后销毁自己
      return;
    };
    shoot();
  }
}


game::bullet_settings::FlowerBullet::FlowerBullet(){}
FlowerBullet::~FlowerBullet(){}


void game::bullet_settings::FlowerBullet::_bind_methods(){
  godot::ClassDB::bind_method(godot::D_METHOD("get_rad"), &FlowerBullet::get_rad);
  godot::ClassDB::bind_method(godot::D_METHOD("set_rad", "rad"), &FlowerBullet::set_rad);
  godot::ClassDB::add_property("FlowerBullet", godot::PropertyInfo(godot::Variant::FLOAT, "rad"), "set_rad", "get_rad");
  
  godot::ClassDB::bind_method(godot::D_METHOD("get_count"), &FlowerBullet::get_count);
  godot::ClassDB::bind_method(godot::D_METHOD("set_count", "count"), &FlowerBullet::set_count);
  godot::ClassDB::add_property("FlowerBullet", godot::PropertyInfo(godot::Variant::INT, "count"), "set_count", "get_count");

  godot::ClassDB::bind_method(godot::D_METHOD("get_max"), &FlowerBullet::get_max);
  godot::ClassDB::bind_method(godot::D_METHOD("set_max", "max"), &FlowerBullet::set_max);
  godot::ClassDB::add_property("FlowerBullet", godot::PropertyInfo(godot::Variant::FLOAT, "max"), "set_max", "get_max");

  godot::ClassDB::bind_method(godot::D_METHOD("get_min"), &FlowerBullet::get_min);
  godot::ClassDB::bind_method(godot::D_METHOD("set_min", "min"), &FlowerBullet::set_min);
  godot::ClassDB::add_property("FlowerBullet", godot::PropertyInfo(godot::Variant::FLOAT, "min"), "set_min", "get_min");

  godot::ClassDB::bind_method(godot::D_METHOD("get_direction_count"), &FlowerBullet::get_direction_count);
  godot::ClassDB::bind_method(godot::D_METHOD("set_direction_count", "direction_count"), &FlowerBullet::set_direction_count);
  godot::ClassDB::add_property("FlowerBullet", godot::PropertyInfo(godot::Variant::INT, "direction_count"), "set_direction_count", "get_direction_count");

  godot::ClassDB::bind_method(godot::D_METHOD("get_launch_angle"), &FlowerBullet::get_launch_angle);
  godot::ClassDB::bind_method(godot::D_METHOD("set_launch_angle", "launch_angle"), &FlowerBullet::set_launch_angle);
  godot::ClassDB::add_property("FlowerBullet", godot::PropertyInfo(godot::Variant::FLOAT, "launch_angle"), "set_launch_angle", "get_launch_angle");
}
double FlowerBullet::get_rad() const {
  return rad;
}
void FlowerBullet::set_rad(double p_rad) {
  this->rad = p_rad;
}
int FlowerBullet::get_count() const {
  return count;
}
void FlowerBullet::set_count(int p_count) {
  this->count = p_count;
}
double FlowerBullet::get_max() const {
    return max;
}
void FlowerBullet::set_max(double p_max) {
    this->max = p_max;
}
double FlowerBullet::get_min() const {
    return min;
}
void FlowerBullet::set_min(double p_min) {
    this->min = p_min;
}
int FlowerBullet::get_direction_count() const {
    return direction_count;
}
void FlowerBullet::set_direction_count(int p_direction_count)
{
  this->direction_count = p_direction_count;
}