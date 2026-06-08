#include "../game/bullet_manager.hpp"

#include "godot_cpp/classes/wrapped.hpp"

namespace game::bullet_settings{
class Strafe : public game::BulletManager{
  GDCLASS(Strafe, game::BulletManager)
private:
  int frame_counter = 0;
  double sweep_range_deg = 100; // 扫射的总扇形角度（60度）
  double current_offset_deg = 30; // 从相对中心的左半边开始扫（-30度）
  double sweep_direction = 1.0;   // 1.0 为正向扫，-1.0 为反向扫
  public:
  void shoot() override;
  static void _bind_methods(){};
  Strafe();
  ~Strafe() = default;
};
}