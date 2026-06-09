#pragma once

#include "enemy.hpp"

namespace game{

class Boos : public Enemy {
  GDCLASS(Boos, Enemy);

private:
protected:
  // 当前符卡阶段 0 则为道中
  int level = 0;
  // 当前状态
  enum status{
    moveing,
    shooring,
    waiting,
  };
  static void _bind_methods(){};
public:
  Boos() = default;
  ~Boos() = default;
};
}