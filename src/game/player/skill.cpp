#include "skill.hpp"
#include "../../conf/bullet.hpp"
#include "../enemy/enemy_manager.hpp"
#include "../enemy/enemy.hpp"


#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/core/math.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/vector2.hpp"
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/atlas_texture.hpp>
#include <godot_cpp/classes/circle_shape2d.hpp>

using namespace game::player;
using namespace godot;

void Skill::check_collisions(){
}

void Skill::_bind_methods(){
  godot::ClassDB::bind_method(godot::D_METHOD("_on_area_entered", "other_area"), &game::player::Skill::_on_area_entered);
}

void Skill::_on_area_entered(godot::Area2D *other_area){
  auto other = Object::cast_to<game::Enemy>(other_area);
  if (!other) return;
  other->hp -= 100;
  if (is_boon && is_to_boon == 0){
    is_to_boon = live_frame + 30;
  }
}

void Skill::_physics_process(double delta){
  // 检测是否要炸
  if ((is_to_boon > 0 && is_to_boon <= live_frame) || live_frame > 300) {
    queue_free();
  }
  if (live_frame <= 200){
    if (live_frame <= 15){
      speed += 0.5;
    }else if (live_frame <= 60){
      angle_speed += 0.01;
      speed = Math::max(0.0, speed-0.5);
    }else if (130 <= live_frame){
      angle_speed = Math::max(0.05, angle_speed-0.01);
      speed = Math::min(1.0, speed+0.5);
    }
    godot::Vector2 old_pos = get_position();
    angle += angle_speed;
    lenght += speed;
    set_position(Vector2(lenght*cos(angle),lenght*sin(angle)));
    vector_speed = get_position() - old_pos;
  }else if(live_frame <= 205){
    set_position(get_position()+vector_speed);
  }else{
    is_boon = true;
    auto ene_man = enemy::EnemyManager::get_singleton();
    if (!ene_man) {
      UtilityFunctions::print("Skill::_physics_process EnemyManager not found");
      return;
    }

    // 跟踪最近的敌人
    godot::Vector2 bullet_pos = get_global_position();
    godot::Node2D* enemy = ene_man->get_closest_enemy(bullet_pos, 10000);
    if (enemy != nullptr) {
      double speed_length = vector_speed.length();
      godot::Vector2 target_pos = enemy->get_global_position();
      godot::Vector2 ideal_direction = (target_pos - bullet_pos).normalized();
      godot::Vector2 current_direction = vector_speed / speed_length;
      godot::Vector2 new_direction = current_direction.lerp(ideal_direction, 0.2).normalized();

      vector_speed = new_direction * speed_length;
    }
    set_global_position(get_global_position() + vector_speed);
    set_global_rotation(vector_speed.angle());
  }
  live_frame++;
}

void game::player::Skill::_ready(){
    // 清除所有碰撞层
  set_collision_layer(0);
  set_collision_mask(0);
  // 设置自身为第几层
  set_collision_layer_value(1, true);
  // 检测第哪几层
  set_collision_mask_value(3, true);
  // 连接内置信号 "area_entered" 到本对象的指定函数
  connect("area_entered", godot::Callable(this, "_on_area_entered"));

  String path = String(conf::player::skill_1::path.c_str());
  double scale = conf::player::skill_1::scale;
  double radius = conf::player::skill_1::radius;
  // 设置技能贴图
  ResourceLoader* loader = ResourceLoader::get_singleton();
  Ref<AtlasTexture> texture = loader->load(path);
  if (!texture.is_valid()) {
    godot::UtilityFunctions::print(path, " load erro");
    return;
  }
  spr = memnew(Sprite2D);
  spr->set_texture(texture);
  spr->set_scale(Vector2(1,1)*scale);
  add_child(spr);
  // 设置碰撞
  Ref<CircleShape2D> circle_shape = memnew(godot::CircleShape2D);
  circle_shape->set_radius(radius);
  auto coll = memnew(CollisionShape2D);
  coll->set_shape(circle_shape);
  add_child(coll);
}

Skill::Skill(){}

Skill::Skill(double angle) : angle(angle) {}

Skill::~Skill(){}