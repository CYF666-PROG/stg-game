#include "boss_magic.hpp"
#include "godot_cpp/classes/atlas_texture.hpp"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/resource_loader.hpp>

using namespace godot;
using namespace game::enemy;

void BossMagic::_bind_methods() {}

BossMagic::BossMagic() {}

BossMagic::~BossMagic() {}

void BossMagic::_ready() {
  // 1. 默认隐藏并把大小、透明度设为 0
  set_visible(false);
  set_scale(Vector2(0, 0));

  Color current_modulate = get_modulate();
  current_modulate.a = 0.0f; // 初始完全透明
  set_modulate(current_modulate);

  // 2. 动态载入 AtlasTexture
  Ref<AtlasTexture> res = ResourceLoader::get_singleton()->load("res://material/enemy/boos/boss_magic.tres");
  if (res.is_valid()) {
    set_texture(res);
  } else {
    UtilityFunctions::print("res://material/enemy/boos/boss_magic.tres load error");
  }
}

void BossMagic::_physics_process(double delta) {
  if (current_state == STATE_EXPANDING) {
    // 🔄 1. 持续旋转
    set_rotation(get_rotation() + rotate_speed * delta);

    // ⏱️ 2. 累加呼吸时间轴
    pulse_time += delta * pulse_speed;

    // 🌊 3. 计算呼吸带来的动态缩放增量
    // Math::sin 会在 -1.0 到 1.0 之间循环，乘以振幅后就在 -pulse_amplitude 到 +pulse_amplitude 之间变化
    float current_pulse = Math::sin(pulse_time) * pulse_amplitude;
    
    // 最终波动的目标大小 = 基础大小 + 呼吸增量
    float final_target_scale = target_scale + current_pulse;

    // 📐 4. 使用 lerp 逼近这个“动态波动”的目标，实现平滑的展开 + 变大变小
    Vector2 current_scale = get_scale();
    float next_scale_x = Math::lerp(current_scale.x, final_target_scale, (float)(expand_speed * delta));
    float next_scale_y = Math::lerp(current_scale.y, final_target_scale, (float)(expand_speed * delta));
    set_scale(Vector2(next_scale_x, next_scale_y));

    // 👻 5. 透明度渐现逻辑：随着法阵变大，透明度也逐步逼近设定的 max_alpha
    Color current_modulate = get_modulate();
    current_modulate.a = Math::lerp(double(current_modulate.a), double(max_alpha), (double)(expand_speed * delta));
    set_modulate(current_modulate);
  }
}

// 🔷 方法 1：呈现法阵展开效果
void BossMagic::deploy() {
  // 重置各项状态，确保每次开大都有完美的初始弹射展开动态
  set_scale(Vector2(0, 0)); 
  
  Color current_modulate = get_modulate();
  current_modulate.a = 0.0f; // 从纯透明开始淡入
  set_modulate(current_modulate);

  pulse_time = 0.0;          // 重置呼吸时间轴，防止波形断层
  set_visible(true);
  
  current_state = STATE_EXPANDING;
}

// 🔷 方法 2：法阵不显示/收回
void BossMagic::retract() {
  set_visible(false);
  set_scale(Vector2(0, 0));
  
  Color current_modulate = get_modulate();
  current_modulate.a = 0.0f;
  set_modulate(current_modulate);
  
  current_state = STATE_HIDDEN;
}