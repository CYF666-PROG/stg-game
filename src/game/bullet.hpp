#pragma once

#include "entity.hpp"
#include "../utility/move.hpp"

#include <godot_cpp/classes/sprite2d.hpp>

#include <vector>
#include <memory>

namespace game {
class Bullet : public Entity{
  GDCLASS(Bullet, Entity)
private:
public:
  /// @brief 轨迹对象数组
  std::vector<std::unique_ptr<utility::Move>> moves ;
  /// @brief 出生坐标 之后会动态修改作为当前坐标
  godot::Vector2 start_position{20,20};

  static void _bind_methods();
  virtual void move(double delta);
  bool hit_wall();
  virtual void entity_physics_process(double delta) override;
  virtual void _ready() override ;

  Bullet();
  virtual ~Bullet();
};
}