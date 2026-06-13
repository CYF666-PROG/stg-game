#pragma once

#include "../entity.hpp"
#include "godot_cpp/variant/string.hpp"

#include <godot_cpp/core/class_db.hpp>

namespace game{
 /// 敌人 
class Enemy : public Entity{
  GDCLASS(Enemy, Entity)
private:
  static void _bind_methods();
public:
  godot::String animation_path = "";
  // 设置贴图和碰撞
  void set_coll_sprite_frames(godot::String path, double d, double scale);
  // 当另一个 Area2D 进入时触发
  virtual void _on_area_entered(godot::Area2D *other_area);
  virtual void _ready() override;
  Enemy();
  virtual ~Enemy();
};
}