#pragma once

#include "../bullet.hpp"

namespace game::bullet {

class RoundBullet : public Bullet {
  GDCLASS(RoundBullet, Bullet);
private:
protected:
  static void _bind_methods();
public:
  /// 是否激活
  bool is_visible = false;
  // 激活并初始化子弹状态
  void init(godot::Vector2 p_start_pos);
  // 停用子弹
  void deactivate();
  virtual void dead() override;
  void _ready() override;
  RoundBullet();
  virtual ~RoundBullet();
};

} // namespace game::bullet
