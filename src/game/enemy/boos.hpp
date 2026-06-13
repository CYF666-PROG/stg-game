#pragma once

#include "boss_magic.hpp"
#include "enemy.hpp"
#include "../../ui/boss_health.hpp"
#include "../../ui/card_name.hpp"
#include "../levelmanager.hpp"

#include "godot_cpp/classes/animated_sprite2d.hpp"
#include "godot_cpp/variant/vector2.hpp"
#include <functional>

namespace game{

class Boos : public Enemy {
  GDCLASS(Boos, Enemy);

private:
  double befor_hp = 5000;
public:
  double max_hp = 5000;
  game::enemy::BossMagic* boss_magic = nullptr;
  game::LevelManager* level_manager = nullptr;
  ui::CardName* card_name = nullptr;
  ui::BossHealth* health = nullptr;
  godot::AnimatedSprite2D* anima = nullptr;
  // 符卡切换时无敌帧
  int invincible_frame = 0 ;
  // 当前符卡阶段 0则为道中 1为到达1符中间 2为1符卡
  int level = 0;
  // 当前状态
  enum Status{
    moveing,
    shooting,
    waiting,
  };
  // boos当前状态
  Status status = moveing;
  Status befor_status = moveing;
  // 用于标识某个动作持续的帧数 配合live_frame使用
  int frame_status = 0;
   // 记录实时速度向量
  godot::Vector2 current_velocity = godot::Vector2(0, 0);
  // 射击lamb
  std::function<void(Boos*)> shoot_l = nullptr;
  // 移动lamb
  std::function<void(Boos*)> move_l = nullptr;
  // 等待lamb
  std::function<void(Boos*)> wait_l = nullptr;
  // 移动工具函数 返回true则为移动完成
  bool move(godot::Vector2 p_target);
  // 切换符卡
  virtual void next();
  // 切换符卡时全屏消弹 同时清理符卡名
  virtual void clear();
  // 死亡
  virtual void dead() override;
  // 符卡阶段 0 为道中
  virtual void start_0(){};
  virtual void start_1(){};
  virtual void start_2(){};
  virtual void start_3(){};
  virtual void start_4(){};
  virtual void start_5(){};
  virtual void start_6(){};
public:
  void update_animation() override;
  void set_level(int level);
  void entity_physics_process(double delta) override;
  virtual void hit_bullet() override;
  static void _bind_methods(){};
  void _ready() override;
  Boos() = default;
  virtual ~Boos() = default;
};
}