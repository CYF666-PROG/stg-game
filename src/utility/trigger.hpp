#pragma once

#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/core/class_db.hpp>


namespace utility{
class Trigger : public godot::Area2D{
  GDCLASS(Trigger, godot::Area2D)
private:

public:
  enum EmitterTYP{
    FlowerBullet
  };
  EmitterTYP emitter_typ = FlowerBullet;
  /// @brief 发射次数
  int count = 0;
  /// @brief 发射间隔
  double fire_rate = 2;

  int get_count();
  double get_fire_rate();
  EmitterTYP get_emitter_typ();
  void set_count(int count);
  void set_fire_rate(double fire_rate);
  void set_emitter_typ(EmitterTYP emitter_typ);
  static void _bind_methods();
  Trigger();
  ~Trigger();
};
} // namespace utility
