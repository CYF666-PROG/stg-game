#pragma once

#include "../entity.hpp"
#include "godot_cpp/variant/string.hpp"
#include "../bullet_pool.hpp"

#include <godot_cpp/core/class_db.hpp>

namespace game{
 /// 敌人 
class Enemy : public Entity{
  GDCLASS(Enemy, Entity)
private:
  static void _bind_methods();
public:
  game::BulletPool* pool = nullptr;
  godot::String animation_path = "";
  // 添加掉落物
  void add_dropped_items(double hp_up, double star_up, double power_up);
  // 设置贴图和碰撞
  void set_coll_sprite_frames(godot::String path, double d, double scale);
  // 当另一个 Area2D 进入时触发
  virtual void _on_area_entered(godot::Area2D *other_area);
  virtual void _ready() override;
  Enemy();
  virtual ~Enemy();
};
}