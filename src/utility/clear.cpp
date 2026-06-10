#include "clear.hpp"

using namespace godot;
using namespace utility;

void Clear::_physics_process(double delta) {
  if (circle_shape.is_valid()) {
    // 获取当前半径
    float current_radius = circle_shape->get_radius();
    if (current_radius >= 1100) {
      queue_free();
    }
    // 每帧增加 10px
    float new_radius = current_radius + 50;
    circle_shape->set_radius(new_radius);
  }
};

void Clear::_ready(){
  // 清除所有碰撞层
  set_collision_layer(0);
  set_collision_mask(0);
  // 设置自身为第几层
  set_collision_layer_value(1, true);
    // 实例化 CollisionShape2D 节点
  collision_shape = memnew(CollisionShape2D);
  add_child(collision_shape);

  // 实例化 CircleShape2D 资源
  circle_shape.instantiate();
  circle_shape->set_radius(10.0); // 设置初始半径为 10px

  // 将圆形形状分配给碰撞节点
  collision_shape->set_shape(circle_shape);
};