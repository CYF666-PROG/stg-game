#include "entity.hpp"
#include "audio_manager.hpp"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/circle_shape2d.hpp>

using namespace game ;
using namespace godot;

void game::Entity::dead(){
  queue_free();
}

void game::Entity::entity_physics_process(double delta){}

void game::Entity::hit_bullet(){
  hp -= 1;
}

void Entity::_bind_methods() {}

void game::Entity::update_animation(){}

void game::Entity::_physics_process(double delta){
  /// 检查是否处于编辑器
  if (godot::Engine::get_singleton()->is_editor_hint()){
    return;
  }
  // 计算这一帧 Area2D 的实际移动速度
  Vector2 current_position = get_global_position();
  speed = (current_position - last_position); 
  last_position = current_position; // 更新历史坐标
  entity_physics_process(delta);
}

void game::Entity::_ready(){}

Entity::Entity(){
  this->keyboard = input::KeyBoard::get_singleton();
  audio = AudioManager::get_audio();
  if (!audio) {
    UtilityFunctions::print("Entity::Entity() AudioManager not foud");
  }
}

Entity::~Entity(){
  this->keyboard = nullptr;
}