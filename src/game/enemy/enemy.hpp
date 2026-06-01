#pragma once

#include "../entity.hpp"
#include "../bullet_manager.hpp"

#include <godot_cpp/core/class_db.hpp>

namespace game{
 /// 敌人 
class Enemy : public Entity{
  GDCLASS(Enemy, Entity)
private:
  static void _bind_methods();
public:
  /// 当另一个 Area2D 进入时触发
  virtual void _on_area_entered(godot::Area2D *other_area);

  virtual void _ready() override;
  Enemy();
  virtual ~Enemy();
};
}