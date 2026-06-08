#pragma once

#include "bullet.hpp"
#include "bullet_pool.hpp"

#include <godot_cpp/classes/node2d.hpp>
#include "godot_cpp/classes/sprite_frames.hpp"
#include "godot_cpp/variant/vector2.hpp"


#include <vector>

namespace game{

/// 用于发射弹幕
class BulletManager : public godot::Node2D {
  GDCLASS(BulletManager, godot::Node2D)
private:
protected :
  BulletPool* pool = nullptr ;
  static void _bind_methods();   
  std::vector<Bullet*> active_bullets{nullptr};
  /// @brief 开始射击后过去的帧数
  int frame = 0;
public:
  enum bull_Typ{
    Pointed,
    Ring,
    Circle,
    fire
  };
  bull_Typ bull_typ = Pointed;
  /// 发射次数
  int fire_count = 4;
  /// 发射间隔 帧
  int fire_interval = 1;
  // 延迟多少帧
  int fire_delay = 0;
  /// 发射弹幕贴图
  godot::Ref<godot::SpriteFrames> to_launch_texture;
  /// 贴图旋转偏移
  double rotation_offset = 0;
  /// 贴图缩放
  godot::Vector2 zoom = godot::Vector2(1,1);
  /// 判定点大小
  double radius = 200;
  virtual void shoot();
  virtual void shoot_effects();
  void load_bullet_infor();
  virtual void frame_do(double delta);

  // 声明 Getter & Setter
  void set_fire_count(const int p_count){fire_count = p_count;};
  int get_fire_count() {return fire_count;}
  void set_fire_interval(const int p_interval){fire_interval = p_interval;}
  int get_fire_interval() {return fire_interval;}
  void set_to_launch_texture(const godot::Ref<godot::SpriteFrames> p_texture){to_launch_texture = p_texture;}
  void set_fire_type(int p_type) { bull_typ = (bull_Typ)p_type; }
  int get_fire_type() const { return (int)bull_typ; }
  godot::Ref<godot::SpriteFrames> get_to_launch_texture() const{return to_launch_texture;}
  void set_fire_delay(int x){fire_delay = x;}
  int get_fire_delay(){return fire_delay;}

  virtual void _physics_process(double delta) override;
  virtual void _ready() override;
  BulletManager();
  virtual ~BulletManager();
};
} // namespace game

VARIANT_ENUM_CAST(game::BulletManager::bull_Typ);