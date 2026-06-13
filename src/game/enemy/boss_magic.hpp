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

  // 🌟 控制参数（可以导出到编辑器或在 C++ 中直接改）
  double target_scale = 3.0;      // 法阵展开后的最终缩放大小
  double rotate_speed = 6.0;      // 每秒旋转的角度（弧度制）
  double expand_speed = 5.0;      // 展开时的平滑插值速度（越大越快）

  // 🌟 新增：呼吸律动（缩放微调）参数
  double pulse_amplitude = 0.5;  // 呼吸振幅（在基本缩放基础上增加/减少的最大缩放值，如 0.15）
  double pulse_speed = 4.0;       // 呼吸频率（数值越大，变大变小的频率越快）
  double pulse_time = 0.0;        // 律动计时器

  // 🌟 新增：最大透明度控制
  double max_alpha = 0.5;         // 法阵完全展开后的透明度上限 (0.0 到 1.0)

  double current_scale_progress = 0.0f;

protected:
  static void _bind_methods();

public:
  BossMagic();
  ~BossMagic();

  void _ready() override;
  void _physics_process(double delta) override;
  // 展开法阵
  void deploy();
  // 收回/隐藏法阵
  void retract();
};

} // namespace game