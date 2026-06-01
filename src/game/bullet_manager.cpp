#include "bullet_manager.hpp"

#include <godot_cpp/classes/engine.hpp>

using namespace game;

void game::BulletManager::_bind_methods(){}

void game::BulletManager::shoot() {}

void game::BulletManager::frame_do(double delta){}

void game::BulletManager::_physics_process(double delta){
  ///检查是否处于编辑器
  if (godot::Engine::get_singleton()->is_editor_hint()){
    return;
  }
  frame_do(delta);
}

void game::BulletManager::_ready(){}

BulletManager::BulletManager() {}

BulletManager::~BulletManager(){}
