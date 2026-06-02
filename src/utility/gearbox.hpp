#pragma once

#include "../game/enemy/minion.hpp"

#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/core/class_db.hpp>

#include <map>

namespace utility{
class Gearbox : public godot::Area2D{
  GDCLASS(Gearbox, godot::Area2D)
private:
  // 对象指针 采用ObjectID 防止对象中途被摧毁
  struct MinionHitInfo {
    int64_t object_id;     // 存储 ObjectID 对应的整型 ID
    double initial_speed = 0;  // 记录挨撞那一瞬间的初始速度，初始设为 -1 表示还未初始化
  };

  /// @brief 累计帧数
  int frame = 0;
  /// @brief 碰到时累计的帧数 
  std::multimap<int, MinionHitInfo> hited_minion;
  /// @brief 延迟 秒
  double delay = 0;
  /// @brief 目标速度 像素每秒
  double target_speed = 100;
  /// @brief 达到目标速度所用时间
  double time_taken = 100;
public:
  /// 返回真则为变速完成
  bool change_speed(int hit_frame, game::enemy::Minion* minion, double initial_speed);
  virtual void _on_area_entered(godot::Area2D *other_area);
  static void _bind_methods();
  void _physics_process(double delta);
  void _ready() override;
  Gearbox();
  ~Gearbox();

  // Delay 的 Getter / Setter
  void set_delay(const double p_delay);
  double get_delay() const;

  // Target Speed 的 Getter / Setter
  void set_target_speed(const double p_speed);
  double get_target_speed() const;

  // Time Taken 的 Getter / Setter
  void set_time_taken(const double p_time);
  double get_time_taken() const;
};
} // namespace utility
