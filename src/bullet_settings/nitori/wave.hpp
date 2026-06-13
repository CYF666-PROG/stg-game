#include "../../conf/bullet.hpp"
#include "../../game/bullet_manager.hpp"

#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/sprite_frames.hpp"
#include "godot_cpp/variant/vector2.hpp"

namespace game::bullet_settings::nitori{
class Wave{
private:
  // (漂溺 "水底粼光，心中痛伤") 符卡单个发射点的发射器
  game::BulletPool* pool = nullptr;
  int frame_counter = 0; //帧计数器
  
  // 炎弹数据
  godot::Ref<godot::SpriteFrames> fire_tex;
  double fire_scale = conf::bullet::fire::scale;
  double fire_radius = conf::bullet::fire::radius;
  double fire_rotation_offset = conf::bullet::fire::rotation_offset;
public:
  game::AudioManager* audio = nullptr;
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