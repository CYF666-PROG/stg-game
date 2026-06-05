#include "flower_bullet.hpp"
#include "../game/bullet/round_bullet.hpp"
#include "../game/bullet_pool.hpp"

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
  shoot_effects();
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
      // auto* pool = game::BulletPool::get_pool();
      // if (!pool) return;
      // auto* sub_bullet = pool->get_bullet();
      // if (!sub_bullet) return;
      // auto move = std::make_unique<utility::Move>(velocity_vector);
      // move->start_time = 0;
      // move->end_time = 1000;
      // sub_bullet->moves.push_back(std::move(move));
      // sub_bullet->init(get_global_position());
      auto *new_pool = BulletPool::get_pool();
      if (!new_pool) {
        UtilityFunctions::print("FlowerBullet::shoot pool not foud");
        return;
      }
      Vector2 spawn_pos = get_global_position(); // 玩家当前位置
      Vector2 direction(0, -1);                  // 向上发射
      float speed = 10;                      // 子弹速度
      float pian = 1.5 ;// 子弹贴图旋转偏移
      // 完全加速帧数
      int jia = 10 ;
      auto linear_behavior = [velocity_vector, speed, pian, jia](BulletPool::Bullet& b){
        b.rotation = pian + b.velocity.angle();
        if (b.lifetime < jia){
          b.velocity = (velocity_vector ) * (double(b.lifetime)/double(jia));
        }else{
          b.velocity = velocity_vector;
        }
        
      };

      // 1. 获取资源加载器的单例
      godot::ResourceLoader* loader = godot::ResourceLoader::get_singleton();
      // 2. 直接加载资源并进行安全强转
      godot::Ref<SpriteFrames> texture = loader->load("res://material/bullet/fire/1.tres");
      // 3. 检查是否加载成功
      if (!texture.is_valid()) {
        godot::UtilityFunctions::print("res://material/bullet/fire/1.tres", " load erro");
        return;
      }

      // 压入弹幕池
      new_pool->spawn(
        spawn_pos,     
        linear_behavior, 
        texture,  
        "normal",
        6.0f,
        1,
        godot::Vector2(1.5,1.5)
      );
    }
  }
}

void game::bullet_settings::FlowerBullet::frame_do(double delta){
  if (fire_count == 0 || int(fire_interval*60) == 0) return;
  if (frame % int(fire_interval*60) == 0){
    if (!fire_count--) {
      queue_free(); // 发射完毕后销毁自己
      return;
    };
    shoot();
  }
  frame += 1;
}


FlowerBullet::FlowerBullet(double rad, int direction_count,int count, double max, double min) : rad(rad), direction_count(direction_count),count(count), max(max), min(min) {}
game::bullet_settings::FlowerBullet::FlowerBullet(){}
FlowerBullet::~FlowerBullet(){}

void game::bullet_settings::FlowerBullet::_ready() {}

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