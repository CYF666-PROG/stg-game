#include "round_bullet.hpp"

#include <godot_cpp/classes/collision_shape2d.hpp>

void game::bullet::RoundBullet::_bind_methods()
{
}

void game::bullet::RoundBullet::init(godot::Vector2 p_start_pos){
  this->start_position = p_start_pos;
  set_position(start_position);
  // 2. 激活节点底层处理
  this->set_process(true);
  this->set_physics_process(true);
  this->set_visible(true);
  // 激活碰撞体
  // 1. 获取当前节点的所有子节点数量
  int child_count = this->get_child_count();
  // 2. 循环遍历每一个子节点
  for (int i = 0; i < child_count; ++i) {
    godot::Node* child = this->get_child(i);
    // 3. 尝试将子节点转换为 CollisionShape2D 类型
    auto* collision_shape = godot::Object::cast_to<godot::CollisionShape2D>(child);
    // 4. 安全检查：如果是碰撞体，则安全地开启它
    if (collision_shape) {
        collision_shape->set_deferred("disabled", false);
    }
  }
}

void game::bullet::RoundBullet::deactivate(){
  // 1. 清空旧的轨迹，释放智能指针内存
  this->moves.clear(); 
  // 2. 彻底关闭引擎层面的消耗
  this->set_process(false);
  this->set_physics_process(false);
  this->set_visible(false);
  // 禁用碰撞体
  int child_count = this->get_child_count();
  // 2. 循环遍历每一个子节点
  for (int i = 0; i < child_count; ++i) {
    godot::Node* child = this->get_child(i);
    // 3. 尝试将子节点转换为 CollisionShape2D 类型
    auto* collision_shape = godot::Object::cast_to<godot::CollisionShape2D>(child);
    // 4. 安全检查：如果是碰撞体，则安全地开启它
    if (collision_shape) {
        collision_shape->set_deferred("disabled", true);
    }
  }
  is_visible = false;
}

void game::bullet::RoundBullet::dead(){
  deactivate();
}

void game::bullet::RoundBullet::_ready(){
  set_texture_coll("res://material/bullet/bul1-0.png", 2);
}

game::bullet::RoundBullet::RoundBullet(){
}

game::bullet::RoundBullet::~RoundBullet(){
}
