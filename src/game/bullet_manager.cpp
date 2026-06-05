#include "bullet_manager.hpp"
#include "effect_manager.hpp"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/sprite_frames.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;
using namespace game;

void game::BulletManager::_bind_methods(){}

void game::BulletManager::shoot() {}

void game::BulletManager::shoot_effects(){
    // 1. 获取资源加载器的单例
  godot::ResourceLoader* loader = godot::ResourceLoader::get_singleton();
  // 2. 直接加载资源并进行安全强转
  godot::Ref<SpriteFrames> texture = loader->load("res://material/special_effects/bullet/white_creat.tres");
  // 3. 检查是否加载成功
  if (!texture.is_valid()) {
    godot::UtilityFunctions::print("res://material/special_effects/bullet/white_creat.tres", " load erro");
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
      inst.rotation += 0;
      if (inst.total_elapsed_ticks < 3){
        inst.modulate.a = 0;
        inst.scale.x = 5 ;
      }
      // 2. 每帧缩小 2%，直到缩小到零
      inst.scale.x = Math::max(0.0, inst.scale.x - 0.5);
      inst.scale.y = inst.scale.x;
      // 3. 同时逐渐变透明
      inst.modulate.a = Math::min(1.0, inst.modulate.a + 0.1);

      // 当缩小到零时提前销毁
      if (inst.modulate.a == 1) {
          return false;
      }
      return true; // 继续存活
    }
  );
}

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
