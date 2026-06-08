#pragma once

#include "../input/keyboard.hpp"
#include "audio_manager.hpp"

#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>

#include <string>


namespace game
{
/// @brief 实体
class Entity : public godot::Area2D {
  GDCLASS(Entity, godot::Area2D)
private:
protected:
  // 音频总管
  AudioManager* audio = nullptr;
  /// @brief 已存活帧数
  double live_frame = 0;
  input::KeyBoard* keyboard = nullptr;
  godot::Vector2 spead = godot::Vector2(0, 0);
public:
  double hp = 100;
  virtual void entity_physics_process(double delta);
  virtual void hit_bullet();
  /// @param d 碰撞圆半径
  /// @param scale 缩放
  void set_texture_coll(
    const godot::String path, 
    double d,
    godot::Vector2 scale = godot::Vector2(1,1)
  );
  /// @param tex 贴图
  /// @param d 碰撞圆半径
  /// @param scale 缩放
  void set_texture_coll(
    const godot::Ref<godot::Texture2D> tex, 
    double d, 
    godot::Vector2 scale = godot::Vector2(1,1)
  );

  virtual void update_animation();
  void _physics_process(double delta) override;
  virtual void _ready() override ;
  static void _bind_methods();
  Entity();
  virtual ~Entity();
public:
  virtual void dead();
};

} // namespace game::entity
