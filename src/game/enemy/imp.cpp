#include "imp.hpp"
#include "../../bullet_settings/flower_bullet.hpp"
#include "../bullet/player_bullet.hpp"



#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>


using namespace game::enemy;

void Imp::touch_trigger(utility::Trigger* trigger){
  godot::UtilityFunctions::print("touch_trigger");
  if (!trigger) return;
  game::BulletManager* target_shape = nullptr;
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
        target_shape = shape;
        break; // 找到了，跳出循环
      }
    }
  }
  if (!target_shape) return ;
  game::BulletManager* new_bull_emitter = godot::Object::cast_to<game::BulletManager>(target_shape->duplicate());
  if(!new_bull_emitter) return;
  // 设置发射次数和间隔
  new_bull_emitter->fire_count = trigger->count;
  new_bull_emitter->fire_interval = trigger->fire_rate;
  //开启发射器
  new_bull_emitter->set_process_mode(godot::Node::PROCESS_MODE_INHERIT);
  new_bull_emitter->set_process(true);
  new_bull_emitter->set_physics_process(true);
  new_bull_emitter->set_visible(true);
  add_child(new_bull_emitter);
}

/// 碰撞检测
void Imp::_on_area_entered(godot::Area2D *other_area){
  if (other_area == nullptr) return ;
  // 撞到玩家子弹
  game::Bullet* bullet = godot::Object::cast_to<game::Bullet>(other_area);
  if (
    bullet != nullptr && 
    //撞到的是玩家发出的
    godot::Object::cast_to<game::bullet::PlayerBullet>(bullet) != nullptr
  ) {
    bullet->dead();
  }
  // 撞到触发器
  utility::Trigger* trigger = godot::Object::cast_to<utility::Trigger>(other_area);
  if (trigger){
    touch_trigger(trigger);
  }
}

void Imp::entity_physics_process(double delta){
  set_position(start_position);
  for (int i = 0; i < moves.size(); i++){
    if (
      moves[i] &&
      moves[i]->start_time*60 < live_frame &&
      live_frame < moves[i]->end_time*60
    ){
      moves[i]->move(start_position, delta);
    }
  }
  // 获取当前的像素进度
  double current_progress = path_follow->get_progress();
  // 加上这一帧应该移动的距离（速度 * 时间）
  current_progress += speed/60;
  // 重新赋值给 PathFollow2D，它会自动计算并更新子节点的位置
  path_follow->set_progress(current_progress);
  live_frame += 1 ;
}

void Imp::_ready(){
  game::Enemy::_ready();

  set_position(start_position);
  set_texture_coll("res://material/enemy/enemy1-0.png", 15, godot::Vector2(2.1,2.1));
  game::bullet_settings::FlowerBullet* flower_bullet = memnew(game::bullet_settings::FlowerBullet(0.5, 12, 10, 1, 0.5));
  flower_bullet->fire_interval = 0.2;
  flower_bullet->fire_count = 100 ;
  // add_child(flower_bullet);
}

Imp::Imp(){}

Imp::~Imp(){}