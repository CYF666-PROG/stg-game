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
  // Rad (弧度/半径)
  double get_rad() const;
  void set_rad(double p_rad);

  // Count (数量)
  int get_count() const;
  void set_count(int p_count);

  // Max (最大值)
  double get_max() const;
  void set_max(double p_max);

  // Min (最小值)
  double get_min() const;
  void set_min(double p_min);
  // Direction Count (方向总数，常用于弹幕散射角度划分)
  int get_direction_count() const;
  // 关键：声明这个用于生成文档和描述的静态虚函数
  void set_direction_count(int p_direction_count);
};
  
} // namespace game::bullet_settings
