#include "enemy.hpp"
#include "enemy_manager.hpp"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace game;
using namespace godot;

void game::Enemy::_bind_methods(){
  // 注册 _on_area_entered
  godot::ClassDB::bind_method(godot::D_METHOD("_on_area_entered", "other_area"), &game::Enemy::_on_area_entered);
}

void game::Enemy::_on_area_entered(godot::Area2D *other_area){}



void game::Enemy::_ready(){
  // 清除所有碰撞层
  set_collision_layer(0);
  set_collision_mask(0);
  // 设置自身为第3层
  set_collision_layer_value(3, true);
  // 检测第2，5层
  set_collision_mask_value(2, true);
  set_collision_mask_value(5, true);
  // 连接内置信号 "area_entered" 到本对象的指定函数
  connect("area_entered", godot::Callable(this, "_on_area_entered"));
  // 注册到敌人管理器
  auto enm_man = enemy::EnemyManager::get_singleton();
  if (!enm_man){
    UtilityFunctions::print("Enemy::_ready EnemyManager not fond");
    return;
  }
  enm_man->register_enemy(this);
}

Enemy::Enemy(){}

Enemy::~Enemy(){}