#include "flower_bullet.hpp"
#include "../game/bullet/round_bullet.hpp"
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

  // 🎯 花弹的初始角度
  double base_angle = 0;
  // 大于7则跟踪
  if (launch_angle >= 185) {
    base_angle = get_global_position().angle_to_point(player->get_global_position());
  }else {
    base_angle = godot::Math::deg_to_rad(launch_angle);
  }

  // 🌟 角度控制参数
  double angle_step = godot::Math::deg_to_rad(rad);            // 每隔多少弧度发射一次
  int angle_count = direction_count;  // 一共发射多少个方向

  // 计算速度的步长间隔
  double speed_step = 0.0;
  if (count > 1) {
      speed_step = (max - min) / (count - 1);
  }

  // 先行检查游戏场景树
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

  // 1. 外层循环：控制不同的发射角度（以 base_angle 为中心对称发射）
  for (int j = 0; j < angle_count; j++) {
      
      // 💡 核心对称数学公式：
      // 计算当前索引 j 相对于中心点的偏移系数
      // 奇数时：j = 0 得到 -(count-1)/2，正好对应最左边；中间项正好为 0（正中心）
      // 偶数时：j = 0 得到 -count/2 + 0.5，两侧完美对称，正中心留空
      double angle_offset = (j - (angle_count - 1) / 2.0) * angle_step;
      
      // 计算当前方向的绝对弧度
      double current_angle = base_angle + angle_offset;
      
      // 根据当前弧度计算方向向量
      godot::Vector2 direction_vector(cos(current_angle), sin(current_angle));
      
      // 2. 内层循环：在当前方向上，生成 count 个速度均匀的子弹
      for (int i = 0; i < count; i++) {
          // 计算当前子弹的均匀速度
          double current_speed = min + (i * speed_step);
          if (count == 1) {
              current_speed = max;
          }
          
          // 将方向乘以速度，得到最终的物理速度向量
          godot::Vector2 velocity_vector = direction_vector * current_speed;
          
          auto *new_pool = BulletPool::get_pool();
          if (!new_pool) {
              UtilityFunctions::print("FlowerBullet::shoot pool not found");
              return;
          }
          
          Vector2 spawn_pos = get_global_position(); // 玩家/发射源当前位置
          
          // 💡 移除了原先这里冲突且没用到的 Vector2 direction(0, -1) 和 speed = 10
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

          if (to_launch_texture.is_null()) return;

          // 压入弹幕池
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


FlowerBullet::FlowerBullet(double rad, int direction_count,int count, double max, double min) : rad(rad), direction_count(direction_count),count(count), max(max), min(min) {}
game::bullet_settings::FlowerBullet::FlowerBullet(){}
FlowerBullet::~FlowerBullet(){}


void game::bullet_settings::FlowerBullet::_bind_methods(){
// ---- [1. Rad (弧度/半径) 绑定] ----
  // 绑定 Getter 和 Setter 方法
  godot::ClassDB::bind_method(godot::D_METHOD("get_rad"), &FlowerBullet::get_rad);
  godot::ClassDB::bind_method(godot::D_METHOD("set_rad", "rad"), &FlowerBullet::set_rad);
  // 注册为编辑器属性 (C++ double 对应 Variant::FLOAT)
  godot::ClassDB::add_property("FlowerBullet", godot::PropertyInfo(godot::Variant::FLOAT, "rad"), "set_rad", "get_rad");
  

  // ---- [2. Count (子弹数量) 绑定] ----
  godot::ClassDB::bind_method(godot::D_METHOD("get_count"), &FlowerBullet::get_count);
  godot::ClassDB::bind_method(godot::D_METHOD("set_count", "count"), &FlowerBullet::set_count);
  // 注册为编辑器属性
  godot::ClassDB::add_property("FlowerBullet", godot::PropertyInfo(godot::Variant::INT, "count"), "set_count", "get_count");

  // ---- [3. Max (最大值) 绑定] ----
  godot::ClassDB::bind_method(godot::D_METHOD("get_max"), &FlowerBullet::get_max);
  godot::ClassDB::bind_method(godot::D_METHOD("set_max", "max"), &FlowerBullet::set_max);
  // 注册为编辑器属性
  godot::ClassDB::add_property("FlowerBullet", godot::PropertyInfo(godot::Variant::FLOAT, "max"), "set_max", "get_max");

  // ---- [4. Min (最小值) 绑定] ----
  godot::ClassDB::bind_method(godot::D_METHOD("get_min"), &FlowerBullet::get_min);
  godot::ClassDB::bind_method(godot::D_METHOD("set_min", "min"), &FlowerBullet::set_min);
  // 注册为编辑器属性
  godot::ClassDB::add_property("FlowerBullet", godot::PropertyInfo(godot::Variant::FLOAT, "min"), "set_min", "get_min");

  // ---- [5. Direction Count (方向总数) 绑定] ----
  godot::ClassDB::bind_method(godot::D_METHOD("get_direction_count"), &FlowerBullet::get_direction_count);
  godot::ClassDB::bind_method(godot::D_METHOD("set_direction_count", "direction_count"), &FlowerBullet::set_direction_count);
  // 注册为编辑器属性
  godot::ClassDB::add_property("FlowerBullet", godot::PropertyInfo(godot::Variant::INT, "direction_count"), "set_direction_count", "get_direction_count");

  godot::ClassDB::bind_method(godot::D_METHOD("get_launch_angle"), &FlowerBullet::get_launch_angle);
  godot::ClassDB::bind_method(godot::D_METHOD("set_launch_angle", "launch_angle"), &FlowerBullet::set_launch_angle);
  // 注册为编辑器属性
  godot::ClassDB::add_property("FlowerBullet", godot::PropertyInfo(godot::Variant::FLOAT, "launch_angle"), "set_launch_angle", "get_launch_angle");
}
double FlowerBullet::get_rad() const {
  return rad;
}
void FlowerBullet::set_rad(double p_rad) {
  this->rad = p_rad;
}
// 2. Count
int FlowerBullet::get_count() const {
  return count;
}
void FlowerBullet::set_count(int p_count) {
  this->count = p_count;
}
// 3. Max
double FlowerBullet::get_max() const {
    return max;
}
void FlowerBullet::set_max(double p_max) {
    this->max = p_max;
}
// 4. Min
double FlowerBullet::get_min() const {
    return min;
}
void FlowerBullet::set_min(double p_min) {
    this->min = p_min;
}
// 5. Direction Count
int FlowerBullet::get_direction_count() const {
    return direction_count;
}

void FlowerBullet::set_direction_count(int p_direction_count)
{
  this->direction_count = p_direction_count;
}