#pragma once

#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/atlas_texture.hpp>

namespace game::enemy {

class BossMagic : public godot::Sprite2D {
  GDCLASS(BossMagic, godot::Sprite2D)

private:
  enum State {
    STATE_HIDDEN,
    STATE_EXPANDING
  };
  State current_state = STATE_HIDDEN;

  double target_scale = 3.0;      // 展开后的最终缩放大小
  double rotate_speed = 6.0;      // 每秒旋转的角度 弧度
  double expand_speed = 5.0;      // 展开时的平滑插值速度

  double pulse_amplitude = 0.5;   // 呼吸缩放振幅
  double pulse_speed = 4.0;       // 呼吸频率
  double pulse_time = 0.0;        // 呼吸计时器

  double max_alpha = 0.5;         // 法阵完全展开后的最大透明度

  double current_scale_progress = 0.0f;

protected:
  static void _bind_methods();

public:
  BossMagic();
  ~BossMagic();

  void _ready() override;
  void _physics_process(double delta) override;
  // 展开
  void deploy();
  // 收回
  void retract();
};

} // namespace game