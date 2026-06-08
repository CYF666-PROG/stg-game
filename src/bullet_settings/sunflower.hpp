#include "../game/bullet_manager.hpp"

#include "godot_cpp/classes/wrapped.hpp"

namespace game::bullet_settings{
class Sunflower : public game::BulletManager{
  GDCLASS(Sunflower, game::BulletManager)
private:

public:
  void shoot() override;
  void inside();
  void outside();

  static void _bind_methods(){};
  Sunflower();
  ~Sunflower();
};
}