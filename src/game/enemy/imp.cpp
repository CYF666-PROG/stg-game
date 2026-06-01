#include "imp.hpp"
#include "../../bullet_settings/flower_bullet.hpp"
#include "../bullet/player_bullet.hpp"

#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>


using namespace game::enemy;

/// 碰撞检测
void game::enemy::Imp::_on_area_entered(godot::Area2D *other_area){
  if (other_area == nullptr) return ;
  game::Bullet* bullet = godot::Object::cast_to<game::Bullet>(other_area);
  if (
    bullet == nullptr || 
    //撞到的不是玩家发出的
    godot::Object::cast_to<game::bullet::PlayerBullet>(bullet) == nullptr
  ) return ;
  bullet->dead();
}

void Imp::entity_physics_process(double delta){
  set_position(start_position);
  bool to_dead = true; // 大于所有end则dead;
  for (int i = 0; i < moves.size(); i++){
    if (
      moves[i] &&
      moves[i]->start_time*60 < live_frame &&
      live_frame < moves[i]->end_time*60
    ){
      moves[i]->move(start_position, delta);
    }
    if (live_frame < moves[i]->end_time*60){
      to_dead = false;
    }
  }
  if (to_dead) dead();
  
  live_frame += 1 ;
}

void Imp::_ready(){
  game::Enemy::_ready();
  set_position(start_position);
  set_texture_coll("res://material/enemy/enemy1-0.png", 15, godot::Vector2(2.1,2.1));
  game::bullet_settings::FlowerBullet* flower_bullet = memnew(game::bullet_settings::FlowerBullet(0.25, 12, 10, 6, 2));
  flower_bullet->fire_interval = 0.5;
  flower_bullet->fire_count = 60 ;
  add_child(flower_bullet);
}

Imp::Imp(){}

Imp::~Imp(){}