#pragma once

#include <godot_cpp/classes/texture_progress_bar.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace ui {

class BossHealth : public godot::TextureProgressBar {
  GDCLASS(BossHealth, godot::TextureProgressBar);

private:
  // 可以在这里定义一些平滑血条的缓冲变量（可选）
  double target_value = 100.0;
  double fill_speed = 5.0;

protected:
  static void _bind_methods();

public:
  BossHealth();
  ~BossHealth();

  // 在 Godot 节点就绪时进行初始化配置
  void _ready() override;


  /**
  * @brief 每帧由 Boss 调用来更新血条状态
  * @param current_health 当前血量
  * @param max_health 最大血量
  * @param delta 帧间隔时间 (get_process_delta_time)
  */
  void update_health(double current_health, double max_health);
};
}