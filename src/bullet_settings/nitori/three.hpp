#include "../../game/bullet_manager.hpp"
#include "../../conf/bullet.hpp"

#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/sprite_frames.hpp"
#include "godot_cpp/classes/wrapped.hpp"

namespace game::bullet_settings::nitori{
  // 水符 "河童的幻想大瀑布"
class Three : public game::BulletManager{
  GDCLASS(Three, game::BulletManager)
private:
  game::BulletPool* pool = nullptr;
  int frame_counter = 0; //帧计数器
  
  double time_accumulator = 0.0; // 驱动第一阶段角度摆动的时间轴

  double spawn_min_x = 72;     // 屏幕左边界
  double spawn_max_x = 936;    // 屏幕右边界
  int interval_frames = 1;        // 每隔多少帧发射

  double target_y_base = 234.0f;   // 目标 Y 轴中心点
  double target_y_range = 60.0f;   // 目标 Y 轴随机范围（234 ± 30）

  double stage1_base_deg = 90.0f;  // 基础方向
  double stage1_swing_amp = 30.0f; // 摆动幅度（左右各 35 度）
  double stage1_swing_speed = 1;// 摆动速度

  double bullet_max_speed = 5.0f;  // 子弹正常飞行速度
  double stop_duration = 60;     // 在中间停顿的总时长（帧）

  // 炎弹数据
  godot::Ref<godot::SpriteFrames> fire_tex; // 炎弹贴图
  double fire_scale = conf::bullet::fire::scale;
  double fire_radius = conf::bullet::fire::radius;
  double fire_rotation_offset = conf::bullet::fire::rotation_offset;
public:
  void shoot() override;
  void re_set();
  static void _bind_methods(){};
  Three();
  ~Three() = default;
};
}