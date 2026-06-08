#include "../game/bullet_manager.hpp"

#include "godot_cpp/classes/wrapped.hpp"

namespace game::bullet_settings{
class NitoriTwo : public game::BulletManager{
  GDCLASS(NitoriTwo, game::BulletManager)
private:
public:
  void shoot() override;

  static void _bind_methods(){};
  NitoriTwo() = default;
  ~NitoriTwo() = default;
};
}