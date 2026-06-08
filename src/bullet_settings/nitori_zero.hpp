#include "../game/bullet_manager.hpp"

#include "godot_cpp/classes/wrapped.hpp"

namespace game::bullet_settings{
class NitoriZero : public game::BulletManager{
  GDCLASS(NitoriZero, game::BulletManager)
private:
  int pointed_frame_counter = 0;
  double pointed_current_base_degree = 0.0; // 记录弹幕的当前基础旋转角度
  int pointed_burst_counter = 0;       // 记录当前是第几批

  int fire_frame_counter = 0;
  double fire_current_base_degree = 0.0; // 记录弹幕的当前基础旋转角度
  int fire_burst_counter = 0;       // 记录当前是第几批
public:
  void fire();
  void pointed();

  void shoot() override;
  static void _bind_methods(){};
  NitoriZero() = default;
  ~NitoriZero() = default;
};
}