#include "godot_cpp/classes/area2d.hpp"
#include "godot_cpp/classes/circle_shape2d.hpp"
#include "godot_cpp/classes/collision_shape2d.hpp"
#include "godot_cpp/classes/wrapped.hpp"

namespace utility{
class Clear : public godot::Area2D{
  GDCLASS(Clear, godot::Area2D)
public:
  godot::CollisionShape2D* collision_shape = nullptr;
  godot::Ref<godot::CircleShape2D> circle_shape;
  void _ready() override;
  void _physics_process(double delta) override;
  static void _bind_methods(){};
  Clear() = default;
  ~Clear() = default;
};
}