#include "imp.hpp"
#include "../bullet/player_bullet.hpp"
#include "../bullet_manager.hpp"
#include "../../conf/bullet.hpp"

#include "godot_cpp/classes/collision_object2d.hpp"
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/animated_sprite2d.hpp>

using namespace godot;
using namespace game::enemy;



void Imp::_ready(){
  Minion::_ready();
  set_coll_sprite_frames(animation_path, conf::minion::imp::radius, conf::minion::imp::scale);
  set_position(start_position);
}

Imp::Imp(){}

Imp::Imp(String path){
  set_coll_sprite_frames(path, 20, 1);
}

Imp::~Imp(){}