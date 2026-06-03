#include "player.hpp"
#include "bullet/player_bullet.hpp"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/animated_sprite2d.hpp>

using namespace godot;

using namespace game ;

void game::Player::update_animation(){
  auto* animation = Object::cast_to<AnimatedSprite2D>(get_node_or_null("animation"));
  if (!animation){
    UtilityFunctions::print("not fond animation");
    return;
  }
  float speed_x = spead.x;
  String current_anim = animation->get_animation();

  // 1. 核心自动接续逻辑：当非循环动画（正向或反向）播放完毕时
  if (!animation->is_playing()) {
    // 【正向起步结束】 -> 进入持续奔跑
    if (current_anim == "to_left" && animation->get_frame() != 0) {
      animation->play("left");
      current_anim = "left";
    } 
    else if (current_anim == "to_right" && animation->get_frame() != 0) {
      animation->play("right");
      current_anim = "right";
    }
    // 【反向刹车结束】 -> 此时动画停在第 0 帧，真正进入静止状态
    else if ((current_anim == "to_left" || current_anim == "to_right") && animation->get_frame() == 0) {
      animation->play("normal");
      current_anim = "normal";
    }
  }

  // 2. 状态机：根据速度方向控制起步和倒带刹车
  if (Math::is_zero_approx(speed_x)) {
    // 【静止状态】
    if (current_anim == "left") {
      // 从左边停下：倒带播放 to_left
      animation->play_backwards("to_left");
    } 
    else if (current_anim == "right") {
      // 从右边停下：倒带播放 to_right
      animation->play_backwards("to_right");
    }
    else if (current_anim != "to_left" && current_anim != "to_right" && current_anim != "normal") {
      // 安全保底：如果既不是在倒带，也不是 normal，就切回 normal
      animation->play("normal");
    }
  } 
  else if (speed_x < 0.0f) {
    // 【向左移动】
    // 如果当前在右边跑、或者在往右倒带，直接打断，触发向左起步
    if (current_anim != "to_left" && current_anim != "left") {
      animation->set_frame(0); // 确保从头正向播放
      animation->play("to_left");
    }
  } 
  else if (speed_x > 0.0f) {
    // 【向右移动】
    if (current_anim != "to_right" && current_anim != "right") {
      animation->set_frame(0); // 确保从头正向播放
      animation->play("to_right");
    }
  }
}

void Player::entity_physics_process(double date){
  keyboard->update(date);
  update();
  move();
  update_animation();
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

void game::Player::_ready(){
  // 清除所有碰撞层
  set_collision_layer(0);
  set_collision_mask(0);
  // 设置自身为第一层
  set_collision_layer_value(1, true);
  // 检测第四层
  set_collision_mask_value(1, true);
  set_collision_mask_value(6, true);
}

game::Player::Player(){
  godot::UtilityFunctions::print("Player::Player()",keyboard);
}
game::Player::~Player(){}