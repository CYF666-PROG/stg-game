#pragma once

#include "../game/bullet_manager.hpp"

using namespace game;

namespace game::bullet_settings{

class FlowerBullet : public BulletManager {
  GDCLASS(FlowerBullet, BulletManager) ///必须有这个和_bind_methods 才会调用_ready等
private:
  double rad = 0.5;
  int count = 5; 
  double max = 6; 
  double min = 2;
  int direction_count = 8; /// 多少方向
protected:
  static void _bind_methods();
public:
  void shoot() override;

  void frame_do(double delta) override;
  void _ready() override ;
  //每隔多少弧度，向多少个方向，发射几颗，最大速度，最小速度
  FlowerBullet(double rad,int direction_count, int count, double max, double min);
  FlowerBullet();
  virtual ~FlowerBullet();
};
  
} // namespace game::bullet_settings
