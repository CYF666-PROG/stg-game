#pragma once

#include "../input/keyboard.hpp"
#include "audio_manager.hpp"

#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>

namespace game{

class UiManager;

/// @brief 实体
class Entity : public godot::Area2D {
  GDCLASS(Entity, godot::Area2D)
private:
protected:
  /// @brief 已存活帧数
  int live_frame = 0;
  input::KeyBoard* keyboard = nullptr;
  // 计算每帧速度的变量
  godot::Vector2 last_position;
  godot::Vector2 speed = godot::Vector2(0, 0);
public:
  UiManager* ui = nullptr;
  // 音频总管
  AudioManager* audio = nullptr;
  double hp = 100;
  virtual void entity_physics_process(double delta);
  virtual void hit_bullet();

  virtual void update_animation();
  void _physics_process(double delta) override;
  virtual void _ready() override ;
  static void _bind_methods();
  Entity();
  virtual ~Entity();
public:
  virtual void dead();
};

}
