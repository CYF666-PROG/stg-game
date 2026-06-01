#pragma once

#include "../bullet.hpp"

namespace game::bullet {

class PlayerBullet : public Bullet {
  GDCLASS(PlayerBullet, Bullet);
private:
protected:
  static void _bind_methods();
public:

  void _ready() override ;

  PlayerBullet();
  virtual ~PlayerBullet();
};

} // namespace game::bullet
