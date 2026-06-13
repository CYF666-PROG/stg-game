#pragma once

#include <godot_cpp/classes/texture_progress_bar.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace ui {

class BossHealth : public godot::TextureProgressBar {
  GDCLASS(BossHealth, godot::TextureProgressBar);

private:
  double target_value = 100.0;
  double fill_speed = 5.0;

protected:
  static void _bind_methods();

public:
  BossHealth();
  ~BossHealth();

  void _ready() override;
  // 更新血条状态 当前血量 最大血量
  void update_health(double current_health, double max_health);
};
}