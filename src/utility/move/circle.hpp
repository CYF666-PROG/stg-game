#pragma once

#include "../move.hpp"

namespace utility::move{
class Circle : public utility::Move{
private:
protected:
  double current_rad = 0.0;        // 累计转过的弧度
public:
  enum TYP {
    Normal,
    Horizontal, /// 仅水平
    Vertical, /// 仅竖直
  };
  TYP typ = Normal;
  /// @brief 旋转总角度
  double rad = 6.28;
  /// @brief 旋转半径 正数为顺时针弯曲（向右拐）
  double d = 300 ;
  /// @brief 总时间 
  double time = 5;
  /// @brief 初始朝向
  godot::Vector2 orientation{0, 1} ;
  virtual void move(godot::Vector2& this_position, double delta) override;

  Circle();
  /// @brief 速度 rad/s 半径 持续时间 开始方向向量x 开始方向向量y
  /// @param spead 速度 rad/s
  /// @param d 半径
  /// @param time 持续时间
  /// @param x 开始方向向量x
  /// @param y 开始方向向量y
  /// @param typ 类型 仅水平还是仅竖直
  Circle(double spead, double d, double time, double x, double y, TYP typ = Normal);
  virtual ~Circle();
};
} // namespace utility::move
