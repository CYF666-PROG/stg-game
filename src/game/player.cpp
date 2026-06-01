#include "player.hpp"
#include "bullet/player_bullet.hpp"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/sprite2d.hpp>

using namespace game ;

void Player::entity_physics_process(double date){
  keyboard->update(date);
  update();
  move();
  if(keyboard->is_shoot) shoot();
}

void Player::update()
{
  if (!keyboard){
    godot::UtilityFunctions::print("Player::update() keyboard nullptr");
    return;
  }
  if (keyboard->is_left){
    spead.x = -10;
  }else if (keyboard->is_right){
    spead.x = 10;
  }else{
    spead.x = 0;
  }
  if (keyboard->is_up){
    spead.y = -10;
  }else if (keyboard->is_down){
    spead.y = 10;
  }else{
    spead.y = 0;
  }
}

void game::Player::move(){
  godot::Vector2 this_pesition = get_position();
  //检查碰撞
  godot::TypedArray<godot::Node2D> hit_array = get_overlapping_bodies();
  for (int i = 0; i < hit_array.size(); i++){
    godot::Node2D* body = godot::Object::cast_to<godot::Node2D>(hit_array[i]);
    if (!body){
      continue;
    }
    if (body->get_name().to_lower().contains("wall_left") && spead.x < 0){
      spead.x = 0 ;
    }else if (body->get_name().to_lower().contains("wall_right") && spead.x > 0){
      spead.x = 0 ;
    }else if (body->get_name().to_lower().contains("wall_up") && spead.y > 0){
      spead.y = 0 ;
    }else if (body->get_name().to_lower().contains("wall_dow") && spead.y < 0){
      spead.y = 0 ;
    }
  }
  set_position(this_pesition + spead);
}

void game::Player::shoot(){
  Bullet* player_bullet = memnew(bullet::PlayerBullet);
  player_bullet->start_position = get_position() + godot::Vector2(0,-54);
  get_parent()->add_child(player_bullet);
}

void game::Player::_bind_methods(){
  godot::ClassDB::bind_method(godot::D_METHOD("set_bullet_texture", "tex"), &game::Player::set_bullet_texture);
  godot::ClassDB::bind_method(godot::D_METHOD("get_bullet_texture"), &game::Player::get_bullet_texture);
  ADD_PROPERTY(godot::PropertyInfo(godot::Variant::OBJECT, "bullet_texture", godot::PROPERTY_HINT_RESOURCE_TYPE, "Texture2D"), "set_bullet_texture", "get_bullet_texture");
}

void game::Player::_ready(){}

game::Player::Player(){
  godot::UtilityFunctions::print("Player::Player()",keyboard);
}
game::Player::~Player(){}