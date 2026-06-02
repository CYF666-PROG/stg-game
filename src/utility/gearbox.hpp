#pragma once

#include "../game/enemy/minion.hpp"

#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/core/class_db.hpp>

#include <map>

namespace utility{
class Gearbox : public godot::Area2D{
  GDCLASS(Gearbox, godot::Area2D)
private:
  /// @brief 累计帧数
  int frame = 0;
  /// @brief 碰到时累计的帧数 对象指针 采用ObjectID 防止对象中途被摧毁
  std::multimap<int, godot::ObjectID> hited_minion;
  /// @brief 延迟 秒
  double delay = 0;
  /// @brief 目标速度 像素每秒
  double target_speed = 100;
  /// @brief 达到目标速度所用距离
  double distance = 100;
public:
  void change_spead();
  virtual void _on_area_entered(godot::Area2D *other_area);
  static void _bind_methods();
  void _physics_process(double delta);
  void _ready() override;
  Gearbox();
  ~Gearbox();
};
} // namespace utility
