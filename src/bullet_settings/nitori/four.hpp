#include "../../game/bullet_manager.hpp"
#include "../../conf/bullet.hpp"

#include "godot_cpp/classes/node2d.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/sprite_frames.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/vector2.hpp"
#include <vector>

namespace game::bullet_settings::nitori{
class Four : public game::BulletManager{
  GDCLASS(Four, game::BulletManager)
private:

  const int interval_frames = 60;       // 每隔多少帧发射这一大波（30颗）
  const int b_count = 60;               // 一次发射的子弹总数
  const double stage1_max_speed = 6;    // 初始发射和最终扩散的最大速度
  const double stage2_max_speed = 5;
  const double target_radius = 200.0f;   // 停下的目标半径
  const int stop_duration_frames = 60;  // 停顿蓄力的帧数
  const double orbit_speed_deg = 1.0f;   // 每帧公转的角度（1度/帧）
  const double orbit_linear_speed = 3; // 公转线速度
  const double rotation_speed_deg = 5 ; // 自转速度
  
  int ring_frame_count = 0; // 射击环时的计时器

  // 椭圆弹数据
  godot::Ref<godot::SpriteFrames> ellipse_tex; // 椭圆弹贴图
  double ellipse_scale = conf::bullet::ellipse::scale;
  double ellipse_radius = conf::bullet::ellipse::radius;
  double ellipse_rotation_offset = conf::bullet::ellipse::rotation_offset;
  // 米弹数据
  godot::Ref<godot::SpriteFrames> rice_tex; // 椭圆弹贴图
  double rice_scale = conf::bullet::rice::scale;
  double rice_radius = conf::bullet::rice::radius;
  double rice_rotation_offset = conf::bullet::rice::rotation_offset;
public:
  void shoot() override;
  void shoot_player();
  void shoot_rings();
  void shoot_ring(
    double base_offset_rad, // 初始偏移弧度
    double target_radius, // 停下时的半径
    double stage2_max_speed, // 停顿后扩散的速度
    int stop_duration_frames  // 停顿的帧数
  );
  static void _bind_methods(){};
  Four();
  ~Four() = default;
};
}