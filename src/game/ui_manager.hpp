#include "godot_cpp/classes/animated_sprite2d.hpp"
#include "godot_cpp/classes/atlas_texture.hpp"
#include "godot_cpp/classes/color_rect.hpp"
#include "player/player.hpp"

#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/color.hpp"
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <vector>

namespace game {
class UiManager : public godot::Node{
  GDCLASS(UiManager, godot::Node)
private:
  static UiManager* the_ui_manager;
  game::Player* player = nullptr;
  godot::ColorRect* status_ui = nullptr;
  std::vector<godot::Ref<godot::AtlasTexture>> hearts_tex;
  std::vector<godot::Ref<godot::AtlasTexture>> star_tex;
  std::vector<godot::Sprite2D*> hearts;
  std::vector<godot::Sprite2D*> stars;
public:
  static UiManager* get_ui_manager();
  bool load_status_ui();
  bool load_heart_card();
  void check_player();
  void _physics_process(double delta) override;
  void _ready() override;
  static void _bind_methods();
  UiManager();
  ~UiManager();
};
}