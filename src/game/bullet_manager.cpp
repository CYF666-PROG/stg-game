#include "bullet_manager.hpp"
#include "effect_manager.hpp"
#include "../conf/bullet.hpp"


#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/sprite_frames.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;
using namespace game;


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
      if (elapsed_ticks < 3){
        inst.modulate.a = 1;
        inst.scale.x = 2 ;
      }
      // 2. 每帧缩小 2%，直到缩小到零
      inst.scale.x = Math::max(0.0, inst.scale.x - 0.2);
      inst.scale.y = inst.scale.x;
      // 3. 同时逐渐变不透明
      inst.modulate.a = Math::min(1.0, inst.modulate.a - 0.1);

      if (inst.scale.x == 0) {
          return false;
      }
      return true; // 继续存活
    }
  );
}

void game::BulletManager::frame_do(double delta){
  if (frame > fire_delay) {
    shoot();
  }
}

void game::BulletManager::_physics_process(double delta){
  ///检查是否处于编辑器
  if (godot::Engine::get_singleton()->is_editor_hint()){
    return;
  }
  frame_do(delta);
  ++frame;
}

void game::BulletManager::_ready(){
  load_bullet_infor();
  // 1. 禁用物理帧更新（对应 _physics_process）
  set_physics_process(false);
  // 2. 禁用闲置帧更新（对应 _process）
  set_process(false);
  pool = BulletPool::get_pool();
  if (!pool) {
    godot::UtilityFunctions::print("BulletManager::_ready pool not found");
  }
  audio_manager = AudioManager::get_audio();
  if (!audio_manager) {
    godot::UtilityFunctions::print("BulletManager::_ready audio_manager not found");
  }
  player = get_node<game::Player>("/root/play/Player");
  if(!player) {
    godot::UtilityFunctions::print("BulletManager::_ready player not found");
  };
}

void BulletManager::start_shoot(){
  // 物理帧更新（对应 _physics_process）
  set_physics_process(true);
  // 闲置帧更新（对应 _process）
  set_process(true);
}


BulletManager::BulletManager() {}

BulletManager::~BulletManager(){}

void BulletManager::load_bullet_infor(){
  if (bull_typ == Pointed) {
    radius = conf::bullet::pointed::radius;
    zoom *= conf::bullet::pointed::scale;
    rotation_offset = conf::bullet::pointed::rotation_offset;
  }else if (bull_typ == Ring) {
    radius = conf::bullet::ring::radius;
    zoom *= conf::bullet::ring::scale;
    rotation_offset = conf::bullet::ring::rotation_offset;
  }else if (bull_typ == Circle) {
    radius = conf::bullet::circle::radius;
    zoom *= conf::bullet::circle::scale;
    rotation_offset = conf::bullet::circle::rotation_offset;
  }else if (bull_typ == fire) {
    radius = conf::bullet::fire::radius;
    zoom *= conf::bullet::fire::scale;
    rotation_offset = conf::bullet::fire::rotation_offset;
  }
}

void game::BulletManager::_bind_methods(){
  // ==========================================
  // 1. 注册所有方法 (注意 D_METHOD 的参数名字)
  // ==========================================
  ClassDB::bind_method(D_METHOD("set_fire_count", "p_count"), &BulletManager::set_fire_count);
  ClassDB::bind_method(D_METHOD("get_fire_count"), &BulletManager::get_fire_count);

  ClassDB::bind_method(D_METHOD("set_fire_interval", "p_interval"), &BulletManager::set_fire_interval);
  ClassDB::bind_method(D_METHOD("get_fire_interval"), &BulletManager::get_fire_interval);

  ClassDB::bind_method(D_METHOD("set_to_launch_texture", "p_texture"), &BulletManager::set_to_launch_texture);
  ClassDB::bind_method(D_METHOD("get_to_launch_texture"), &BulletManager::get_to_launch_texture);

  ClassDB::bind_method(D_METHOD("set_fire_delay", "fire_delay"), &BulletManager::set_fire_delay);
  ClassDB::bind_method(D_METHOD("get_fire_delay"), &BulletManager::get_fire_delay);

  // ==========================================
  // 2. 暴露属性到 Godot 检查器 (Inspector)
  // ==========================================

  // 暴露 int: 发射次数
  ClassDB::add_property(get_class_static(),
  PropertyInfo(Variant::INT, "fire_count"),
  "set_fire_count",
  "get_fire_count"
  );

  ClassDB::add_property(get_class_static(),
  PropertyInfo(Variant::INT, "fire_interval"),
  "set_fire_interval",
  "get_fire_interval"
  );

  // 暴露 资源对象 (对应 Variant::OBJECT): 弹幕贴图
  // 极其重要：第三个参数必须是 PROPERTY_HINT_RESOURCE_TYPE，第四个参数写明引擎中的类名 "SpriteFrames"
  ClassDB::add_property(get_class_static(),
  PropertyInfo(Variant::OBJECT, "to_launch_texture", PROPERTY_HINT_RESOURCE_TYPE, "SpriteFrames"),
  "set_to_launch_texture",
  "get_to_launch_texture"
  );

    // 1. 快速绑定方法
  ClassDB::bind_method(D_METHOD("set_fire_type", "p_type"), &BulletManager::set_fire_type);
  ClassDB::bind_method(D_METHOD("get_fire_type"), &BulletManager::get_fire_type);

  // 2. 【最简暴露】使用以逗号分隔的字符串，直接决定 Godot 编辑器下拉菜单里显示什么文字
  // 这里的顺序 "Normal,Shotgun,Laser" 严格对应枚举中 0, 1, 2 的顺序
  ClassDB::add_property(get_class_static(),
      PropertyInfo(godot::Variant::INT, "fire_type", godot::PROPERTY_HINT_ENUM, "pointed,ring,circle,fire"),
      "set_fire_type",
      "get_fire_type"
  );

  // 暴露 延迟
  ClassDB::add_property(get_class_static(),
  PropertyInfo(Variant::INT, "fire_delay"),
  "set_fire_delay",
  "get_fire_delay"
  );
}