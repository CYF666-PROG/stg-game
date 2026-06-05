#include "minion.hpp"
#include "../effect_manager.hpp"

#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/path_follow2d.hpp>
#include <godot_cpp/classes/sprite_frames.hpp> 
#include <godot_cpp/classes/resource_loader.hpp>

using namespace godot;
using namespace game::enemy ;

void game::enemy::Minion::dead(){
  // 1. 获取资源加载器的单例
  godot::ResourceLoader* loader = godot::ResourceLoader::get_singleton();
  // 2. 直接加载资源并进行安全强转
  godot::Ref<SpriteFrames> texture = loader->load("res://material/special_effects/entity_dead.tres");
  // 3. 检查是否加载成功
  if (!texture.is_valid()) {
    godot::UtilityFunctions::print("res://material/special_effects/entity_dead.tres", " load erro");
    return;
  }
  auto eff = game::EffectManager::get_singleton();
  if (!eff){
    godot::UtilityFunctions::print("Minion::dead EffectManager not fond");
    return;
  }
  eff->spawn_effect(
    texture,get_global_position(),
    "normal", 
    0.5,
    [](EffectManager::EffectInstance &inst, int elapsed_ticks) -> bool {
      // --- 只专注于修改数值，非常简便 ---
      // 1. 每帧旋转 0.1 弧度
      inst.rotation += 0.1;
      // 2. 每帧缩小 2%，直到缩小到零
      inst.scale.x = inst.scale.x + 0.3;
      inst.scale.y = inst.scale.x;
      if (inst.total_elapsed_ticks < 3){
        inst.modulate.a = 0.5;
      }
      // 3. 同时逐渐变透明
      inst.modulate.a = Math::max(0.0, inst.modulate.a - 0.05);

      // 当缩小到零时提前销毁
      if (inst.modulate.a == 0) {
          return false;
      }
      return true; // 继续存活
    }
  );
  queue_free();
}

void game::enemy::Minion::update_animation()
{
  auto* animation = Object::cast_to<AnimatedSprite2D>(get_node_or_null("animation"));
  if (!animation){
    UtilityFunctions::print("Minion::update_animation not fond animation");
    return;
  }
  double speed_x = global_speed.x; 
  double abs_speed_x = std::abs(speed_x); 
  const double IDLE_THRESHOLD = 2; 
  // 1. 获取当前正在播放的动画名
  godot::StringName current_anim = animation->get_animation();

  if (abs_speed_x < IDLE_THRESHOLD) { 
    // 【静止状态】
    // 只有当 to_right 播完了，或者当前不是 to_right 时，才能切回 normal 动画
    // 防止人在减速时，转身动画还没播完就被 normal 强行打断
    if (current_anim != godot::StringName("to_right") || !animation->is_playing()) {
      animation->play("normal"); 
    }
  } 
  else { 
    // 【移动状态】处理水平镜像翻转 
    godot::Vector2 current_scale = animation->get_scale(); 
    if (speed_x > 0) { 
      current_scale.x = std::abs(current_scale.x); // 面向右 
    } else { 
      current_scale.x = -std::abs(current_scale.x); // 面向左 
    } 
    animation->set_scale(current_scale); 

    // 2. 核心：控制 to_right 播放完再播 right
    if (current_anim == godot::StringName("normal")) {
      // 如果刚从静止起步，触发转身过渡动画
      animation->play("to_right");
    } 
    else if (current_anim == godot::StringName("to_right")) {
      // 如果当前正在播转身，检查它是否已经放到了最后一帧（或者已经停止播放）
      // 只有满足这个条件，才代表 to_right 播完了
      int current_frame = animation->get_frame();
      int total_frames = animation->get_sprite_frames()->get_frame_count("to_right");

      if (current_frame >= total_frames - 1) {
        // 完美衔接持续移动动画
        animation->play("right");
      }
    } 
    else if (current_anim != godot::StringName("right")) {
      // 兜底保障：如果处于移动状态，且既不是 normal 也不是 to_right，则播放 right
      animation->play("right");
    }
  }
}

void game::enemy::Minion::update_speed(){
  godot::Vector2 current_position = get_global_position();
  // 计算两帧之间的位移距离，像素/帧
  if (last_global_pos != godot::Vector2()) {
    global_speed = current_position - last_global_pos;
  }
  // 更新上一帧位置
  last_global_pos = current_position;
}

void game::enemy::Minion::entity_physics_process(double delta) {}

Minion::Minion(){}

Minion::~Minion(){}