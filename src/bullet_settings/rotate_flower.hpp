#include "../game/bullet_manager.hpp"

#include "godot_cpp/classes/wrapped.hpp"

namespace game::bullet_settings{
class RotateFlower : public game::BulletManager{
  GDCLASS(RotateFlower, game::BulletManager)
private:
  int frame_counter = 0; // 帧率计时
  double current_base_degree = 0.0f; // 记录当前转到了多少度
public:
  void shoot() override;
  static void _bind_methods(){};
  RotateFlower() = default;
  ~RotateFlower() = default;
};
}