#include "bullet.hpp"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/circle_shape2d.hpp>

using namespace game ;

void game::Bullet::move(double delta){
  set_position(start_position);
  for (int i = 0; i < moves.size(); i++){
    if (moves[i] && moves[i]->start_time*60 < live_frame && live_frame < moves[i]->end_time*60){
      moves[i]->move(start_position, delta);
    }
  }
  live_frame += 1 ;
}

bool game::Bullet::hit_wall(){
  if (
    start_position.x > 1000 ||
    start_position.x < 0 ||
    start_position.y > 1100 ||
    start_position.y < 0
  ){
    return true;
  }
  return false;
}
void game::Bullet::_bind_methods(){}

void game::Bullet::entity_physics_process(double delta){
  move(delta);
  if (hit_wall()){
    dead();
  }
}

void game::Bullet::_ready(){
  // set_texture_coll("res://material/player/p1/rm-hb-0.png", 6);
}

Bullet::Bullet(){}

Bullet::~Bullet(){}