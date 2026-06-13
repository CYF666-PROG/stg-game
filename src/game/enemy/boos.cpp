#include "boos.hpp"
#include "../../utility/clear.hpp"
#include "../bullet_manager.hpp"
#include "../../game/effect_manager.hpp"

#include "boss_magic.hpp"
#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/classes/resource_loader.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/vector2.hpp"
#include <godot_cpp/classes/scene_tree.hpp>

using namespace game;
using namespace godot;

bool Boos::move(Vector2 p_target){
  godot::Vector2 current_pos = get_global_position();
  float max_speed = 6.0f;       // 最大速度
  float accel = 0.2f;           // 每帧加速度
  float brake_distance = 30.0f; // 刹车距离

  // 当前位置指向终点的绝对方向和总剩余距离
  godot::Vector2 to_target = p_target - current_pos;
  float distance_left = to_target.length();

  // 如果已到达 则直接改变坐标
  if (distance_left <= 2) {
    set_global_position(p_target);
    current_velocity = godot::Vector2(0, 0); // 速度清零
    return true;
  }

  godot::Vector2 move_dir = to_target.normalized();

  float target_speed = max_speed;

  // 进入刹车距离
  if (distance_left < brake_distance) {
    // 减速因子
    float brake_factor = distance_left / brake_distance;
    // 二次方缓动曲线
    brake_factor = brake_factor * brake_factor;
    target_speed = max_speed * brake_factor;
  }

  // 加速与过渡 当前速度向目标速度匀速逼近
  float current_speed_length = current_velocity.length();
  float next_speed_length = godot::Math::move_toward(current_speed_length, target_speed, accel);
  // 组合成最新的速度向量
  current_velocity = move_dir * next_speed_length;

  // 应用位移与到达保护
  if (current_velocity.length() >= distance_left) {
    set_global_position(p_target);
    current_velocity = godot::Vector2(0, 0); // 刹车稳住
    
    // 移动完成
    return true;
  } 
  else {
    // 正常前进
    set_global_position(current_pos + current_velocity);
  }
  return false;
}

void Boos::next(){
  clear(); // 清楚残留弹幕和残留的发射器
  invincible_frame = 100;
  if (level == 0) {
    dead();
  }else if (level == 1) {
    start_2();
  }else if (level == 2) {
    start_3();
  }else if (level == 3) {
    start_4();
  }else if (level == 4) {
    start_5();
  }else if (level == 5) {
    start_6();
  }else if (level == 6) {
    dead();
  }
}

void Boos::set_level(int level){this->level = level;}


void Boos::clear(){
  // 清除魔法阵
  boss_magic->retract();
  // 全屏消弹
  auto clear = memnew(utility::Clear);
  get_tree()->get_current_scene()->add_child(clear);
  clear->set_global_position(get_global_position());
  // 清楚残留发射器
  auto arr = get_children();
  for (auto it : arr) {
    auto a = Object::cast_to<BulletManager>(it);
    if (a) {
      a->queue_free();
    }
  }
  if (card_name) {
    card_name->queue_free();
    card_name = nullptr;
  }
}

void Boos::dead(){
  // 特效
  audio->play("boos_end");

  // 死亡特效
  ResourceLoader* loader = ResourceLoader::get_singleton();
  Ref<SpriteFrames> texture = loader->load("res://material/special_effects/entity_dead.tres");
  if (!texture.is_valid()) {
    UtilityFunctions::print("res://material/special_effects/entity_dead.tres", " load erro");
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
      if (inst.total_elapsed_ticks < 2){
        inst.modulate.a = 0.5;
      }else {
        inst.rotation += 0.1;
        inst.scale.x = inst.scale.x + 0.2;
        inst.scale.y = inst.scale.x;
      }
      // 逐渐变透明
      inst.modulate.a = Math::max(0.0, inst.modulate.a - 0.01);
      // 变透明时销毁
      if (inst.modulate.a == 0) {
        return false;
      }
      return true; // 继续存活
    }
  );
  level_manager->start();
  // 先等待
  status = waiting;
  move_l = [](Boos* b) mutable {
    if (b->move(Vector2(493,-106))) {
      b->queue_free();
    }
  };
  audio->stop_bgm();
}

void Boos::hit_bullet(){
  if(invincible_frame > 0) return;
  hp -= 1;
}

void Boos::update_animation(){
  if (!anima) return;
  bool is_moving = (speed.length_squared() > 0.001); // 判断速度是否不为零

  godot::StringName current_anim = anima->get_animation();

  if (is_moving) {
    // 移动状态 正向播放 move
    if (current_anim != StringName("move")) {
      // 如果从其他动画切过来，从第 0 帧正向播放 move
      anima->play("move", 1.0, false);
    } else {
      // 如果已经在播 move，但由于不循环它会停在最后一帧。
    }
  } else {
    // 停止状态 倒放 move 直至回到 normal
    if (current_anim == StringName("move")) {
      // 检查当前播放速度
      if (anima->get_speed_scale() > 0.0) {
        // 刚停下来，让 move 动画倒放
        //动画名, 播放速度(-1.0代表倒放), from_end=true(如果是倒放则从最后一帧或当前帧往回走)
        anima->play("move", -1.0, true);
      }
      // 检查倒放是否已经到头 回到了第 0 帧
      if (anima->get_frame() == 0) {
        anima->play("normal");
      }
    } else if (current_anim != StringName("normal")) {
      // 如果既没移动，也没在倒放 move，默认回到 normal
      anima->play("normal");
    }
  }
};

void Boos::entity_physics_process(double delta){
  if (befor_hp > 0 && hp <= 0) next();
  // 更新血条
  health->update_health(hp, max_hp);
  // 更新动画
  update_animation();
  // 方便lamb知道自己这个动作持续了多少帧
  if (befor_status != status) {
    frame_status = 0;
  }
  befor_status = status;
  if (status == moveing && move_l) {
    move_l(this);
  }else if (status == shooting && shoot_l) {
    shoot_l(this);
  }else if (status == waiting && wait_l) {
    wait_l(this);
  }

  befor_hp = hp;
  frame_status++;
  if(invincible_frame > 0) invincible_frame--;
}

void Boos::_ready(){
  hp = 5000;
  // 调用父类的准备
  Enemy::_ready();
  // 暂停关卡管理器
  level_manager = game::LevelManager::get_singleton();
  if (!level_manager) {
    UtilityFunctions::print("Boos::_ready level_manager not foud");
  }
  level_manager->pause();
  // 创建boos血条
  health = memnew(ui::BossHealth);
  add_child(health);
  // 创建boss魔法阵
  boss_magic = memnew(game::enemy::BossMagic);
  add_child(boss_magic);
  if (!health) {
    UtilityFunctions::print("Boos::_ready() health ERRO");
  }
  set_global_position(Vector2(141,-118));
  auto target_pos = Vector2(478,202);
  move_l = [target_pos, level = level](Boos* b){
    if(b->move(target_pos)){
      b->level = level;
      b->clear();
      if (level == 1) {
        b->start_1();
      }else {
        b->start_0();
      }
    };
  };
}