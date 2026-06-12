#include "rotate.hpp"
#include "minion.hpp"
#include "../../conf/bullet.hpp"

#include "godot_cpp/classes/animated_sprite2d.hpp"
#include "godot_cpp/core/math.hpp"

using namespace game::enemy;
using namespace godot;

void Rotate::_ready(){
  Minion::_ready();
  set_coll_sprite_frames(animation_path, conf::minion::imp::radius, conf::minion::imp::scale);
  set_position(start_position);
};

void Rotate::update_animation(){
  auto* animation = Object::cast_to<AnimatedSprite2D>(get_node_or_null("animation"));
  if (!animation){
    UtilityFunctions::print("Rotate::update_animation not fond animation");
    return;
  }
  if (!animation->is_playing()) {
    animation->play("normal"); 
  }
  double speed = Math::deg_to_rad(20.0);  // 旋转速度 度/帧

  // 让动画节点自己旋转
  double new_rot = animation->get_rotation() + speed;
  animation->set_rotation(new_rot);
};