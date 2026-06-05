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
  // 注册 _on_area_entered
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
  if ((is_to_boon > 0 && is_to_boon <= live_frame) || live_frame > 480) {
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

    // 1. 获取当前子弹的全局坐标
    godot::Vector2 bullet_pos = get_global_position();

    // 2. 寻找最近的敌人
    godot::Node2D* enemy = ene_man->get_closest_enemy(bullet_pos, 10000);

    if (enemy != nullptr) {
      // 3. 计算子弹当前的绝对速度大小（像素/帧），并保存下来
      double speed_length = vector_speed.length();

      // 4. 计算指向敌人的目标单位方向向量
      godot::Vector2 target_pos = enemy->get_global_position();
      godot::Vector2 ideal_direction = (target_pos - bullet_pos).normalized();

      // 5. 获取子弹当前的飞行单位方向向量
      godot::Vector2 current_direction = vector_speed / speed_length;

      // 6. ✨ 关键：用向量 Lerp 引入物理惯性 ✨
      // 这里的 0.2 是一个每帧固定的转向拉力系数。
      // 子弹不会瞬间对准敌人，而是让原有方向（惯性）每帧只被往敌人方向“拉动” 3.5%。
      // 数值越小（如 0.01），前一阶段的惯性越强，甩尾漂移半径越大；数值越大（如 0.1），转向越急。
      godot::Vector2 new_direction = current_direction.lerp(ideal_direction, 0.2).normalized();

      // 7. 保持原本的速度大小，仅将新方向赋值回 vector_speed
      vector_speed = new_direction * speed_length;

    }
    // 8. 严格按照当前的 vector_speed（像素/帧）进行位移更新，保留其完美惯性
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
    // 1. 获取资源加载器的单例
  ResourceLoader* loader = ResourceLoader::get_singleton();
  // 2. 直接加载资源并进行安全强转
  Ref<AtlasTexture> texture = loader->load(path);
  // 3. 检查是否加载成功
  if (!texture.is_valid()) {
    godot::UtilityFunctions::print(path, " load erro");
    return;
  }
  // 设置贴图精灵
  spr = memnew(Sprite2D);
  spr->set_texture(texture);
  spr->set_scale(Vector2(1,1)*scale);
  add_child(spr);

  // 设置碰撞半径
  Ref<CircleShape2D> circle_shape = memnew(godot::CircleShape2D);
  circle_shape->set_radius(radius);
  auto coll = memnew(CollisionShape2D);
  coll->set_shape(circle_shape);
  add_child(coll);
}

void Skill::chek_coll(){

}

Skill::Skill(){}

Skill::Skill(double angle) : angle(angle) {}

Skill::~Skill()
{
}