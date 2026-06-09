#include "../../game/bullet_manager.hpp"
#include "../../conf/bullet.hpp"

#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/sprite_frames.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/vector2.hpp"

namespace game::bullet_settings::nitori{
class Wave{
private:
  game::BulletPool* pool = nullptr;
  int frame_counter = 0; //帧计数器
  
  godot::Ref<godot::SpriteFrames> fire_tex; // 炎弹贴图
  // 炎弹数据
  double fire_scale = conf::bullet::fire::scale;
  double fire_radius = conf::bullet::fire::radius;
  double fire_rotation_offset = conf::bullet::fire::rotation_offset;
public:
  godot::Vector2 shoot_pos = godot::Vector2(1,1); // 发射位置
  int direction = 1; // 1向左 -1向右
  bool is_enabled = false;  // 是否启用

  void shoot();
  void re_set();
  static void _bind_methods(){};
  Wave();
  ~Wave() = default;
};
}