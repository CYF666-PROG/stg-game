#pragma once

#include "bullet.hpp"

#include <godot_cpp/classes/node2d.hpp>


#include <vector>

namespace game{

/// 用于发射弹幕
class BulletManager : public godot::Node2D {
  GDCLASS(BulletManager, godot::Node2D)
private:
protected :
  static void _bind_methods();   
  std::vector<Bullet*> active_bullets{nullptr};
  /// @brief 开始射击后过去的帧数
  int frame = 0;
public:
  int fire_count = 4; /// 发射次数
  double fire_interval = 1; /// 发射间隔 秒
  /// 发射弹幕贴图
  godot::Ref<godot::Texture2D> to_launch_texture;
  virtual void shoot();

  virtual void frame_do(double delta);
  virtual void _physics_process(double delta) override;
  virtual void _ready() override;
  BulletManager();
  virtual ~BulletManager();
};
} // namespace game
