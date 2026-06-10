#include "../../game/bullet_manager.hpp"

#include "godot_cpp/classes/wrapped.hpp"

namespace game::bullet_settings{
  // 普通
class NitoriOne : public game::BulletManager{
  GDCLASS(NitoriOne, game::BulletManager)
public:
  // 每轮间隔
  const int frame_interval = 3;
  // 每轮最大子弹数量
  int count = 40;
  // 每轮完成后角度偏转
  double deflection_deg = 0;
  // 基准角度
  const double initial_direction_deg = 90;
  // 每轮子弹间隔角度
  double bullet_deg = 7 ;
  // 子弹速度
  const double speed = 6;

  // 当前基准角度
  double now_initial_direction_rad = 0;
  // 当前应发射子弹的数量
  int now_count = 1;
public:
  bool is_end();
  void re_set();
  void disabled();
  void shoot() override;
  void sector();

  static void _bind_methods(){};
  NitoriOne();
  ~NitoriOne();
};
}