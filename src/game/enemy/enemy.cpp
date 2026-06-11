#include "enemy.hpp"
#include "enemy_manager.hpp"
#include "godot_cpp/classes/animated_sprite2d.hpp"
#include "godot_cpp/classes/circle_shape2d.hpp"
#include "godot_cpp/classes/resource_loader.hpp"
#include "godot_cpp/classes/sprite_frames.hpp"
#include "godot_cpp/variant/vector2.hpp"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace game;
using namespace godot;

void game::Enemy::_bind_methods(){
  // 注册 _on_area_entered
  godot::ClassDB::bind_method(godot::D_METHOD("_on_area_entered", "other_area"), &game::Enemy::_on_area_entered);
}

void game::Enemy::_on_area_entered(godot::Area2D *other_area){}

void Enemy::set_coll_sprite_frames(godot::String path, double d, double scale){
  // 1. 获取资源加载器的单例
  godot::ResourceLoader* loader = godot::ResourceLoader::get_singleton();
  // 2. 加载 SpriteFrames 动画资源（注意：资源类在 C++ 中必须用 Ref<> 包裹）
  godot::Ref<godot::SpriteFrames> sprite_frames = loader->load(path);
  // 3. 检查是否加载成功
  if (!sprite_frames.is_valid()) {
      godot::UtilityFunctions::print(path, " load error");
      return;
  }
  /// 创建 AnimatedSprite2D 动态精灵并添加动画资源
  AnimatedSprite2D* animated_sprite = memnew(godot::AnimatedSprite2D);
  animated_sprite->set_sprite_frames(sprite_frames);
  /// 设置缩放
  animated_sprite->set_scale(Vector2(1, 1) * scale);
  add_child(animated_sprite);
  // 一定要设置名字
  animated_sprite->set_name("animation");
  // 4. 【核心】：让动画动起来
  // 假设你的动画资源里包含名为 "default" 的动画（Godot 默认创建的名字）
  if (sprite_frames->has_animation("normal")) {
    animated_sprite->play("normal");
  } else {
    // 如果没有 default，就自动播放第一个有效的动画
    godot::PackedStringArray anim_list = sprite_frames->get_animation_names();
    if (anim_list.size() > 0) {
      animated_sprite->play(anim_list[0]);
    }
  }
  // 1. 使用 memnew 创建 CollisionShape2D 节点本身（容器）
  godot::CollisionShape2D* collision = memnew(godot::CollisionShape2D);
  // 2. 使用 memnew 创建具体的形状资源（核心数据）
  // 注意：Godot 的资源类在 C++ 中必须用 godot::Ref<> 智能指针包裹，否则会导致内存泄漏
  godot::Ref<godot::CircleShape2D> circle_resource = memnew(godot::CircleShape2D);
  // 3. 设置形状资源的物理属性
  circle_resource->set_radius(d);
  // 4. 将形状资源装载进节点容器中
  collision->set_shape(circle_resource);
  add_child(collision);
}


void game::Enemy::_ready(){
  // 清除所有碰撞层
  set_collision_layer(0);
  set_collision_mask(0);
  // 设置自身为第3层
  set_collision_layer_value(3, true);
  // 检测第2，5层
  set_collision_mask_value(2, true);
  set_collision_mask_value(5, true);
  // 连接内置信号 "area_entered" 到本对象的指定函数
  connect("area_entered", godot::Callable(this, "_on_area_entered"));
  // 注册到敌人管理器
  auto enm_man = enemy::EnemyManager::get_singleton();
  if (!enm_man){
    UtilityFunctions::print("Enemy::_ready EnemyManager not fond");
    return;
  }
  enm_man->register_enemy(this);
}

Enemy::Enemy(){}

Enemy::~Enemy(){}