#pragma once

#include "enemy.hpp"
#include "../../utility/move.hpp"

#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/core/class_db.hpp>

#include <vector>
#include <memory>

namespace game::enemy{
/// @brief 小怪
class Minion : public game::Enemy {
  GDCLASS(Minion, game::Enemy)
private:
protected:
  static void _bind_methods(){}
public:
  /// @brief 出生坐标 之后会动态修改作为当前坐标
  godot::Vector2 start_position{20,20};
  /// @brief 轨迹对象数组
  std::vector<std::unique_ptr<utility::Move>> moves ;
  /// 弹幕发射器

  // BulletManager transmitter; // 千万不能包含其他注册节点！

  virtual void entity_physics_process(double delta);
  Minion();
  virtual ~Minion();
};
} // namespace game::enemy