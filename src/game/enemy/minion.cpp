#include "minion.hpp"
#include "../effect_manager.hpp"
#include "../bullet_manager.hpp"

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
  // 播放死亡特效
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
  // 播放死亡音效
  audio->play("imp_dead");
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

void Minion::update_position(){
  if (path_follow->get_progress_ratio() >= 0.999) {
    path_follow->queue_free();
  }
  // 获取当前的像素进度
  double current_progress = path_follow->get_progress();
  // 加上这一帧应该移动的距离（速度 * 时间）
  current_progress += speed/60;
  // 重新赋值给 PathFollow2D，它会自动计算并更新子节点的位置
  path_follow->set_progress(current_progress);
  live_frame += 1 ;
}

void Minion::touch_trigger(utility::Trigger* trigger){
  ///确保是自己轨迹上的触发器
  if (get_parent()->get_parent() != trigger->get_parent()->get_parent()) return;
  if (!trigger) return;

  // 1. 获取所有的子节点列表
  godot::TypedArray<godot::Node> children = trigger->get_children();
    // 2. 遍历子节点
  for (int i = 0; i < children.size(); ++i) {
    godot::Node* child = godot::Object::cast_to<godot::Node>(children[i]);
    if (child) {
      // 3. 核心：尝试将子节点强转为目标类型（例如 CollisionShape2D）
      game::BulletManager* shape = godot::Object::cast_to<game::BulletManager>(child);
      // 如果强转成功，说明这个子节点就是我们要找的类型！
      if (shape) {
        game::BulletManager* new_bull_emitter = godot::Object::cast_to<game::BulletManager>(shape->duplicate());
        if(!new_bull_emitter) return;
        //开启发射器
        add_child(new_bull_emitter);
        new_bull_emitter->set_process_mode(godot::Node::PROCESS_MODE_INHERIT);
        new_bull_emitter->set_process(true);
        new_bull_emitter->set_physics_process(true);
        new_bull_emitter->set_visible(true);
      }
    }
  }
}

void Minion::_on_area_entered(godot::Area2D *other_area){
  if (other_area == nullptr) return ;
  // 撞到触发器
  utility::Trigger* trigger = godot::Object::cast_to<utility::Trigger>(other_area);
  if (trigger){
    touch_trigger(trigger);
  }
};

void Minion::_ready(){}

void game::enemy::Minion::entity_physics_process(double delta) {
  if (hp < 0){
    dead();
  }
  
  set_position(start_position);
  update_speed();
  for (int i = 0; i < moves.size(); i++){
    if (
      moves[i] &&
      moves[i]->start_time*60 < live_frame &&
      live_frame < moves[i]->end_time*60
    ){
      moves[i]->move(start_position, delta);
    }
  }
  update_position();
  update_animation();
}

Minion::Minion(){}

Minion::~Minion(){}