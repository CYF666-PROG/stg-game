#include "text.hpp"
#include "../input/keyboard.hpp"
#include "../game/ui_manager.hpp"

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include <godot_cpp/classes/scene_tree.hpp>

using namespace ui;
using namespace godot;

void Text::play(){
  UtilityFunctions::print("play");
  // 切换场景
  SceneTree *tree = get_tree();
  if (tree) {
    // 2. 调用切换场景方法（传入 Godot 虚拟路径）
    Error err = tree->change_scene_to_file("res://scene/play.tscn");
    if (err != OK) {
      UtilityFunctions::printerr( err);
    }
  }

  auto ui = game::UiManager::get_ui_manager();
  if (!ui) {
    godot::UtilityFunctions::print("Text::play ui not foud");
    return;
  }
  ui->status_typ = game::UiManager::status_Typ::PLAYING;
}

void Text::_physics_process(double delta){
  // 检查是否在编辑器中
  Engine *engine = Engine::get_singleton();
  if (engine && engine->is_editor_hint()) {
    return;
  }

  auto keyboard = input::KeyBoard::get_singleton();
  if (!keyboard) {
    UtilityFunctions::print("Text::_physics_process keyboard not foud");
    return;
  }

  // 处理点击选中事件
  if (is_selected && keyboard->is_determine) {
    UtilityFunctions::print("click");
    if (typ == PLAY) {
      UtilityFunctions::print("play");
      play();
    }else if (typ == QUIT) {
      quit();
    }
  }

  update_animation();
  ++frame;
};

void Text::update_animation(){
  if (is_selected) {
    double speed_multiplier = 0.05; 
    double factor = (std::sin(frame * speed_multiplier) + 1.0f) * 0.5f;
    godot::Color normal_color = godot::Color(1.0f, 1.0f, 1.0f); // 白色
    godot::Color red_color = godot::Color(1.0f, 0.0f, 0.0f); // 红色
    godot::Color current_color = normal_color.lerp(red_color, factor);
    // 更新 Label 的颜色
    add_theme_color_override("font_color", current_color);
  }else {
    godot::Color normal_color = godot::Color(1.0f, 1.0f, 1.0f); // 默认白色
    add_theme_color_override("font_color", normal_color);
  }
}

void Text::selected(){
  is_selected = true;
};
void Text::deselect(){
  is_selected = false;
};

void Text::set_typ(Typ typ){
  this->typ = typ;
};

void Text::quit(){
  SceneTree *tree = get_tree();
  if (tree) {
    tree->quit(0); 
  }
};

void Text::_ready(){
  if (godot_typ == 0) {
    typ = PLAY;
  }if (godot_typ == 1) {
    typ = QUIT;
  }
};
void Text::_bind_methods(){
  ClassDB::bind_method(D_METHOD("get_godot_typ"), &Text::get_godot_typ);
  ClassDB::bind_method(D_METHOD("set_godot_typ", "typ"), &Text::set_godot_typ);

  ClassDB::add_property(
    get_class_static(), 
    PropertyInfo(Variant::FLOAT, "typ"), 
    "set_godot_typ", 
    "get_godot_typ"
  );
};

void Text::set_godot_typ(int typ){godot_typ = typ;}
int Text::get_godot_typ(){return godot_typ;}


Text::Text(Typ typ) : typ(typ){};
Text::Text(){};
Text::~Text(){};