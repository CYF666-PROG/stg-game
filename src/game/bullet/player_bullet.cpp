#include "../bullet.hpp"
#include "player_bullet.hpp"

#include <godot_cpp/variant/utility_functions.hpp>

void game::bullet::PlayerBullet::_bind_methods(){
}

void game::bullet::PlayerBullet::_ready(){
  // 清除所有碰撞层
  set_collision_layer(0);
  set_collision_mask(0);
  // 设置自身为第四层
  set_collision_layer_value(2, true);
  // 检测第一层
  set_collision_mask_value(3, true);
  // set_texture_coll("res://material/player/p1/rm-hb-0.png", 6);
}

game::bullet::PlayerBullet::PlayerBullet(){
  auto move = std::make_unique<utility::Move>(2000, 0, -1);
  move->start_time = 0;
  move->end_time = 1000;
  moves.push_back(std::move(move));
}

game::bullet::PlayerBullet::~PlayerBullet(){
}