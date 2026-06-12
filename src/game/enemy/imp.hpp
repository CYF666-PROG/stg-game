#pragma once

#include "minion.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/sprite_frames.hpp> 

namespace game::enemy{
/// @brief 小妖精
class Imp : public Minion{
  GDCLASS(Imp, Minion)
private:

public:

  static void _bind_methods(){}
  void _ready() override ;
  Imp();
  /// 动画路径
  Imp(godot::String path);
  virtual ~Imp();
};

} // namespace game::enemy