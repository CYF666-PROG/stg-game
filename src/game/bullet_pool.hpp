#pragma once

#include "bullet/round_bullet.hpp"

#include <godot_cpp/classes/node.hpp>

#include <vector>

namespace game{
class BulletPool : public godot::Node2D{
  GDCLASS(BulletPool, godot::Node2D)
private:
  std::vector<bullet::RoundBullet*> bullets;
  int _current_size = 0;
  const int INITIAL_POOL_SIZE = 5000; // 初始预载入数量
  inline static BulletPool* bullet_pool = nullptr;
protected:
  static void _bind_methods();
public:
  static BulletPool* get_pool();
  BulletPool();
  ~BulletPool();
  virtual void _ready() override;
  // 从池中获取一颗可用子弹
  bullet::RoundBullet* get_bullet();
};

} // namespace game
