#pragma once

#include "minion.hpp"
#include "../../utility/trigger.hpp"

#include <godot_cpp/core/class_db.hpp>

namespace game::enemy{
/// @brief 小妖精
class Imp : public Minion{
  GDCLASS(Imp, Minion)
private:

public:
  void touch_trigger(utility::Trigger* trigger);

  static void _bind_methods(){}
  virtual void _on_area_entered(godot::Area2D *other_area) override;
  void entity_physics_process(double delta) override ;
  void _ready() override ;
  Imp();
  virtual ~Imp();
};

} // namespace game::enemy