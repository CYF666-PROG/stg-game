#pragma once

#include "enemy.hpp"
#include "godot_cpp/classes/animated_sprite2d.hpp"
#include "godot_cpp/variant/vector2.hpp"
#include <functional>

namespace game{

class Boos : public Enemy {
  GDCLASS(Boos, Enemy);

private:
protected:
  godot::AnimatedSprite2D* anima = nullptr;
  // 当前符卡阶段 0 则为道中 1为到达1符中间 2为1符卡
  int level = 0;
  // 当前状态
  enum Status{
    moveing,
    shooting,
    waiting,
  };
  Status status = moveing;
  // 用于标识某个动作的开始帧数 配合live_frame使用
  int frame_timer = 0;
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
  // 符卡阶段 0 为道中
  virtual void start_0(){};
  virtual void start_1(){};
  virtual void start_2(){};
  virtual void start_3(){};
  virtual void start_4(){};
  virtual void start_5(){};
  virtual void start_6(){};
public:
  void entity_physics_process(double delta) override;
  static void _bind_methods(){};
  void _ready() override;
  Boos() = default;
  virtual ~Boos() = default;
};
}