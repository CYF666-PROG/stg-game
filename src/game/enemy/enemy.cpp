#include "enemy.hpp"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace game;

void game::Enemy::_bind_methods(){
  // 注册 _on_area_entered
  godot::ClassDB::bind_method(godot::D_METHOD("_on_area_entered", "other_area"), &game::Enemy::_on_area_entered);
}

void game::Enemy::_on_area_entered(godot::Area2D *other_area){}

void game::Enemy::_ready(){
  // 连接内置信号 "area_entered" 到本对象的指定函数
  connect("area_entered", godot::Callable(this, "_on_area_entered"));
}

Enemy::Enemy(){}

Enemy::~Enemy(){}