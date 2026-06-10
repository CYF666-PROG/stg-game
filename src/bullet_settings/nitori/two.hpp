#include "../../game/bullet_manager.hpp"
#include "../../conf/bullet.hpp"
#include "wave.hpp"

#include "godot_cpp/classes/node2d.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/sprite_frames.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/vector2.hpp"
#include <vector>

namespace game::bullet_settings::nitori{
  // 漂溺 "水底粼光，心中痛伤"
class Two : public game::BulletManager{
  GDCLASS(Two, game::BulletManager)
private:
  int global_frame_counter = 0; // 全局帧计数器
  int state = 0;                // 0: 等待/依次开启, 1: 持续发射中, 2: 依次关闭, 3: 循环结束
  
  // 基础配置参数（你可以根据需要修改）
  double min_x = 72;       // 屏幕左侧起始 X 坐标
  double max_x = 936;    // 屏幕右侧结束 X 坐标
  double min_y = 36;       // 屏幕顶部起始 Y 坐标
  double max_y = 1044;    // 屏幕底部结束 Y 坐标
  
  int activate_interval = 20;   // 开启间隔（每隔10帧开启下一个）
  int keep_alive_frames = 360;  // 持续发射时间（帧数，例如3秒 = 180帧）
  int current_cycle = 0; // 记录当前是第几个大周期
  std::vector<std::unique_ptr<game::bullet_settings::nitori::Wave>> left;
  std::vector<std::unique_ptr<game::bullet_settings::nitori::Wave>> right;
public:
  void re_set();
  void _physics_process(double delta) override;
  virtual void _ready() override ;

  static void _bind_methods(){};
  Two();
  ~Two();
};
}