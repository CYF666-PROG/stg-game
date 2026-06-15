#include "enemy.hpp"
#include "enemy_manager.hpp"
#include "../ui_manager.hpp"
#include "../../conf/bullet.hpp"

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
  // 加载资源
  godot::ResourceLoader* loader = godot::ResourceLoader::get_singleton();
  godot::Ref<godot::SpriteFrames> sprite_frames = loader->load(path);
  if (!sprite_frames.is_valid()) {
      godot::UtilityFunctions::print(path, " load error");
      return;
  }
  /// 创建 AnimatedSprite2D 并添加资源
  AnimatedSprite2D* animated_sprite = memnew(godot::AnimatedSprite2D);
  animated_sprite->set_sprite_frames(sprite_frames);
  animated_sprite->set_scale(Vector2(1, 1) * scale);
  add_child(animated_sprite);
  // 一定要设置名字!！!！!！!！!！!！!！!！!！
  animated_sprite->set_name("animation");
  // 设置默认动画
  if (sprite_frames->has_animation("normal")) {
    animated_sprite->play("normal");
  } else {
    // 如果没有 normal，就自动播放第一个有效的动画
    godot::PackedStringArray anim_list = sprite_frames->get_animation_names();
    if (anim_list.size() > 0) {
      animated_sprite->play(anim_list[0]);
    }
  }
  // 设置碰撞
  godot::CollisionShape2D* collision = memnew(godot::CollisionShape2D);
  godot::Ref<godot::CircleShape2D> circle_resource = memnew(godot::CircleShape2D);
  circle_resource->set_radius(d);
  collision->set_shape(circle_resource);
  add_child(collision);
}

void Enemy::add_dropped_items(double hp_up, double star_up, double power_up){
  // 获取自机
  if (!ui) return;
  auto player = ui->get_player();
  if (!player) return;
  // 加载掉落物贴图
  ResourceLoader* loader = ResourceLoader::get_singleton();
  Ref<SpriteFrames> spr_hp = loader->load(String(conf::power::path_hp.c_str()));
  Ref<SpriteFrames> spr_star = loader->load(String(conf::power::path_star.c_str()));
  Ref<SpriteFrames> spr_power = loader->load(String(conf::power::path_p.c_str()));
  if (spr_hp.is_null()|| spr_star.is_null()|| spr_power.is_null() ){
    UtilityFunctions::print("Minion::add_dropped_items not fond spr");
  }
  auto linear_behavior = [player, is_track = false](BulletPool::Bullet& b) mutable{
    const float ATTRACT_RADIUS = 150.0f;
    const float ATTRACT_SPEED = 10.0; 
    if (!player || !player->is_inside_tree()) {
      b.velocity = Vector2(0, 3);
      return;
    }
    // 当前位置到玩家位置的向量
    Vector2 to_player = player->get_global_position() - b.position; 
    float distance = to_player.length(); // 获取当前距离
    if (is_track) {
      // to_player.normalized() 获取方向，再乘以设定的速度
      b.velocity = to_player.normalized() * ATTRACT_SPEED;
    } else {
      b.velocity = Vector2(0, 3); 
      // 离子机近或者自机在屏幕上方
      is_track = (distance <= ATTRACT_RADIUS || player->get_global_position().y < 290);
    }
  };
  auto power_up_fn  = [](Node* n) -> bool {
    auto player = Object::cast_to<game::Player>(n);
    if (!player) return false;
    player->add_power(0.02);
    return true;
  };
  auto hp_up_fn  = [](Node* n) -> bool {
    auto player = Object::cast_to<game::Player>(n);
    if (!player) return false;
    player->add_hp(5);
    return true;
  };
  auto star_up_fn  = [](Node* n) -> bool {
    auto player = Object::cast_to<game::Player>(n);
    if (!player) return false;
    player->add_star(5);
    return true;
  };

  double spawn_radius = 50.0; // 随机掉落半径大小
  // 掉落p点
  for (int i = 0; i < int(power_up); i++) {
    float random_angle = UtilityFunctions::randf() * Math_TAU; 
    float random_distance = UtilityFunctions::sqrt(UtilityFunctions::randf()) * spawn_radius;
    Vector2 offset = Vector2(
      std::cos(random_angle) * random_distance,
      std::sin(random_angle) * random_distance
    );
    Vector2 spawn_pos = get_global_position() + offset;
    pool->spawn(
      spawn_pos, 
      linear_behavior, 
      spr_power, 
      "normal", 
      5, 1, 10, 
      Vector2(1,1) * conf::power::scale, 
      Vector2(0.5,0.5), 
      0, 
      power_up_fn 
    );
  }
  // 掉落星
  for (int i = 0; i < int(star_up); i++) {
    float random_angle = UtilityFunctions::randf() * Math_TAU; 
    float random_distance = UtilityFunctions::sqrt(UtilityFunctions::randf()) * spawn_radius;
    Vector2 offset = Vector2(
      std::cos(random_angle) * random_distance,
      std::sin(random_angle) * random_distance
    );
    Vector2 spawn_pos = get_global_position() + offset;
    pool->spawn(
      spawn_pos, 
      linear_behavior, 
      spr_star, 
      "normal", 
      5, 1, 10, 
      Vector2(1,1) * conf::power::scale, 
      Vector2(0.5,0.5), 
      0, 
      star_up_fn 
    );
  }
  // 掉落心
  for (int i = 0; i < int(hp_up); i++) {
    float random_angle = UtilityFunctions::randf() * Math_TAU; 
    float random_distance = UtilityFunctions::sqrt(UtilityFunctions::randf()) * spawn_radius;
    Vector2 offset = Vector2(
      std::cos(random_angle) * random_distance,
      std::sin(random_angle) * random_distance
    );
    Vector2 spawn_pos = get_global_position() + offset;
    pool->spawn(
      spawn_pos, 
      linear_behavior, 
      spr_hp, 
      "normal", 
      5, 1, 10, 
      Vector2(1,1) * conf::power::scale, 
      Vector2(0.5,0.5), 
      0, 
      hp_up_fn 
    );
  }
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
  // 连接内置信号 "area_entered"
  connect("area_entered", godot::Callable(this, "_on_area_entered"));
  // 注册到敌人管理器
  auto enm_man = enemy::EnemyManager::get_singleton();
  if (!enm_man){
    UtilityFunctions::print("Enemy::_ready EnemyManager not fond");
    return;
  }
  enm_man->register_enemy(this);
  pool = BulletPool::get_pool();
  if (!pool) {
    UtilityFunctions::print("Minion::_ready pool not foud");
  }
}

Enemy::Enemy(){}

Enemy::~Enemy(){}