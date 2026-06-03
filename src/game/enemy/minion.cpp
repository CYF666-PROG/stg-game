#include "minion.hpp"

#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/path_follow2d.hpp>

using namespace godot;
using namespace game::enemy ;

void game::enemy::Minion::update_animation(){
  auto* animation = Object::cast_to<AnimatedSprite2D>(get_node_or_null("animation"));
  if (!animation){
    UtilityFunctions::print("Minion::update_animation not fond animation");
    return;
  }
  double speed_x = spead.x - 1 ;

  String current_anim = animation->get_animation();
  // 1. 核心自动接续逻辑：当非循环动画（to_right）播放完毕时
  if (!animation->is_playing()) {
    // 【正向起步结束】 -> 进入持续奔跑动画 right
    if (current_anim == "to_right" && animation->get_frame() != 0) {
      animation->play("right");
      current_anim = "right";
    } 
    // 【反向刹车结束】 -> 此时动画倒带停在第 0 帧，真正进入静止状态 normal
    else if (current_anim == "to_right" && animation->get_frame() == 0) {
      animation->set_flip_h(false); // 回到正常朝向（可选，取决于你 normal 的设计）
      animation->play("normal");
      current_anim = "normal";
    }
  }

  // 2. 状态机：仅根据速度方向控制起步、奔跑和倒带刹车
  if (Math::is_zero_approx(speed_x)) {
    // 【静止状态】
    if (current_anim == "right") {
      // 无论向左还是向右，停步时都倒带播放 to_right
      // 此时的 flip_h 会保持奔跑时的状态（左或右），从而实现正确的方向倒带
      animation->play_backwards("to_right");
    } 
    else if (current_anim != "to_right" && current_anim != "normal") {
      // 保底安全检查
      animation->play("normal");
    }
  } 
  else if (speed_x < 0.0f) {
    // 【向左移动】
    if (current_anim != "to_right" && current_anim != "right") {
      animation->set_flip_h(true);  // 🌟 关键：开启视觉镜像，全部朝左
      animation->set_frame(0);      // 确保从头正向播放
      animation->play("to_right");
    }
  } 
  else if (speed_x > 0.0f) {
    // 【向右移动】
    if (current_anim != "to_right" && current_anim != "right") {
      animation->set_flip_h(false); // 🌟 关键：关闭视觉镜像，恢复朝右
      animation->set_frame(0);      // 确保从头正向播放
      animation->play("to_right");
    }
  }
}

void game::enemy::Minion::entity_physics_process(double delta) {}

Minion::Minion(){}

Minion::~Minion(){}