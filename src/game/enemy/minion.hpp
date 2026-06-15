#pragma once

#include "enemy.hpp"
#include "../../utility/move.hpp"
#include "../../utility/trigger.hpp"
#include "../bullet_pool.hpp"

#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/path_follow2d.hpp>

#include <vector>
#include <memory>

namespace game::enemy{
/// @brief 小怪
class Minion : public game::Enemy {
  GDCLASS(Minion, game::Enemy)
private:
protected:
  static void _bind_methods(){}
  game::BulletPool* pool = nullptr;
  godot::Vector2 last_global_pos = godot::Vector2(0,0);
  /// @brief 当前帧的全局速度，像素/帧
  godot::Vector2 global_speed = godot::Vector2(0,0);
public:
  // 死亡后的掉落
  double power_up = 0;
  double hp_up = 0;
  double star_up = 0;
  godot::PathFollow2D* path_follow = nullptr;
  /// @brief 每秒多少像素
  double speed = 200;
  /// @brief 出生坐标 之后会动态修改作为当前坐标
  godot::Vector2 start_position{20,20};
  /// @brief 轨迹对象数组
  std::vector<std::unique_ptr<utility::Move>> moves ;
  /// 弹幕发射器

  // BulletManager transmitter; // 千万不能包含其他注册节点！!!!!!!!! 否则godot内存管理河cpp冲突导致崩溃
  virtual void set_animation(godot::String path);
  virtual void dead() override;
  /// 更新动画
  virtual void update_animation() override;
  void update_speed();
  // 根据path2d更新位置
  void update_position();

  virtual void touch_trigger(utility::Trigger* trigger);
  virtual void _on_area_entered(godot::Area2D *other_area) override;
  void _ready() override;
  virtual void entity_physics_process(double delta) override;
  Minion();
  virtual ~Minion();
};
}