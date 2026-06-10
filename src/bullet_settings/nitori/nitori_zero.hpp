#include "../../game/bullet_manager.hpp"
#include "../../conf/bullet.hpp"

#include "godot_cpp/classes/wrapped.hpp"

namespace game::bullet_settings{
class NitoriZero : public game::BulletManager{
  GDCLASS(NitoriZero, game::BulletManager)
private:
  int pointed_frame_counter = 0;
  int pointed_burst_counter = 0;       // 记录当前是第几批
  int pointed_max_cout = 20;          // 一次最大发射个数

  int fire_frame_counter = 0;
  double fire_current_base_degree = 0.0; // 记录弹幕的当前基础旋转角度
  int fire_burst_counter = 0;       // 记录当前是第几批
  int fire_max_cout = 20;          // 一次最大发射个数

  // 子弹数据
  godot::Ref<godot::SpriteFrames> pointed_tex;
  double pointed_scale = conf::bullet::pointed::scale;
  double pointed_radius = conf::bullet::pointed::radius;
  double pointed_rotation_offset = conf::bullet::pointed::rotation_offset;
public:
  void fire();
  void pointed();
  bool is_end();
  void re_set();
  void _ready() override;
  void shoot() override;
  static void _bind_methods(){};
  NitoriZero();
  ~NitoriZero() = default;
};
}