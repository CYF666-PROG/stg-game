#pragma once

#include "../game/bullet_manager.hpp"

using namespace game;

namespace game::bullet_settings{

class FlowerBullet : public BulletManager {
  GDCLASS(FlowerBullet, BulletManager) ///必须有这个和_bind_methods 才会调用_ready等
private:
  double rad = 0.5;  // 每隔多少角度
  int count = 5;   // 发射几颗
  double max = 6;    // 最大速度
  double min = 2;  // 最小速度
  int direction_count = 8;    // 多少方向
  double launch_angle = 0;  // 扇形发射方向 绝对角度 大于185则跟踪
protected:
  static void _bind_methods();
public:
  void shoot() override;

  void frame_do(double delta) override;
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
  // Direction Count 发射方向总数
  int get_direction_count() const;
  void set_direction_count(int p_direction_count);

  double get_launch_angle(){return launch_angle;};
  void set_launch_angle(double launch_angle){this->launch_angle = launch_angle;}
};
  
} // namespace game::bullet_settings
