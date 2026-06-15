#pragma once

#include "audio_manager.hpp"
#include "godot_cpp/classes/atlas_texture.hpp"
#include "godot_cpp/classes/color_rect.hpp"
#include "godot_cpp/classes/label.hpp"
#include "godot_cpp/classes/node2d.hpp"
#include "player/player.hpp"

#include "godot_cpp/classes/wrapped.hpp"
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>

#include <vector>

namespace game {
class UiManager : public godot::Node{
  GDCLASS(UiManager, godot::Node)
private:
  int frame = 0;
  static UiManager* the_ui_manager;
  game::Player* player = nullptr;
  godot::ColorRect* status_ui = nullptr;
  game::AudioManager* audio = nullptr;
  godot::Label* number = nullptr;
  std::vector<godot::Ref<godot::AtlasTexture>> hearts_tex;
  std::vector<godot::Ref<godot::AtlasTexture>> star_tex;
  std::vector<godot::Sprite2D*> hearts;
  std::vector<godot::Sprite2D*> stars;
  std::vector<godot::Label*> menu_label;
  godot::Node2D* menu_node = nullptr;
  int menu_label_index = 0;
  // 上一帧是否按下esc
  bool was_esc_pressed = false;
  bool was_up_pressed = false;
  bool was_dow_pressed = false;
  public:
  enum status_Typ{
    PLAYING,
    PAUSE,
    DEAD,
    TITLE,
  };
  status_Typ status_typ = TITLE;
  void update_menu();
  static UiManager* get_ui_manager();
  bool load_status_ui();
  bool load_menu();
  void check_player();
  void pause();
  void play();
  void dead();
  void quit();
  void restart();
  void updeat_power();
  game::Player* get_player(){return player;};
  void register_player(game::Player* player); // 注册自机
  void logout_player(); // 取消注册自机
  void game_end();

  void _physics_process(double delta) override;
  void _ready() override;
  static void _bind_methods();
  UiManager();
  ~UiManager();
};
}