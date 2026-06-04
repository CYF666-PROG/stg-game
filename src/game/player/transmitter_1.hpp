#include "../bullet_manager.hpp"
#include "../bullet_pool.hpp"
#include "../../input/keyboard.hpp"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/sprite_frames.hpp>

namespace game::player{
class Transmitter_1 : public game::BulletManager {
  GDCLASS(Transmitter_1, game::BulletManager)
private:
  /// 锚点
  godot::Vector2 bullet_anchor = godot::Vector2(0.5,0.5);
  /// 判定半径
  double bullet_radius = 10;
  /// 旋转偏移
  double bullet_rotation_offset = -1.570781325;
  /// 贴图缩放
  double bullet_scale = 1;
  double scale = 1;
  godot::String path ;
  godot::String bullet_path;
  godot::Ref<godot::SpriteFrames> texture;
  game::BulletPool* pool = nullptr;
  input::KeyBoard* keyboard = nullptr;
  godot::Sprite2D *spr = nullptr;
public:
  enum orb_Typ{
    Red,
    Pink,
    Blue
  };
  enum bullet_Typ{
    fast_bullet_1,
    fast_bullet_2,
    fast_bullet_3
  };

  virtual void _ready() override;
  virtual void frame_do(double delta) override;
  virtual void shoot() override;
  Transmitter_1(orb_Typ orb_typ = Red, bullet_Typ bullet_typ = fast_bullet_1);
  virtual ~Transmitter_1();
  static void _bind_methods();
};
}