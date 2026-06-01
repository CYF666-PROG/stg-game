#pragma once

#include <godot_cpp/variant/vector2.hpp>

namespace utility{
/// @brief 轨迹类
class Move{
private:
  godot::Vector2 spead = godot::Vector2(3,3);
protected:
public:
  double start_time = 1 ;
  double end_time = 6 ;
  /// @param this_position 要修改的位置
  /// @param date 一帧过去的时间
  virtual void move(godot::Vector2& this_position, double delta);

  Move();
  /// @param spead 速度 px/s
  /// @param x 方向向量x
  /// @param y 方向向量y
  Move(double spead, double x, double y);
  Move(godot::Vector2 spead);
  virtual ~Move();
};

} // namespace utility
