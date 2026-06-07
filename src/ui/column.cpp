#include "column.hpp"
#include "../input/keyboard.hpp"

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/label.hpp"
#include "godot_cpp/classes/object.hpp"

using namespace ui;
using namespace godot;

void Column::_physics_process(double delta) {
  // 检查是否在编辑器中
  Engine *engine = Engine::get_singleton();
  if (engine && engine->is_editor_hint()) {
    return;
  }

  auto key = input::KeyBoard::get_singleton();
  if (!key) return;
  // 切换按钮
  if (!was_dow_pressed && key->is_down) {
    labs[label_index]->deselect();
    label_index = (label_index+1) % labs.size();
  }else if (!was_up_pressed && key->is_up) {
    labs[label_index]->deselect();
    label_index = (label_index+labs.size()-1) % labs.size();
  }
  labs[label_index]->selected();

  was_dow_pressed = key->is_down;
  was_up_pressed = key->is_up;
};
void Column::_ready(){
  // 获取Label子节点
  TypedArray<Node> children = get_children();
  for (auto &it : children) {
    auto lab = Object::cast_to<Text>(it);
    if (!lab) continue;
    labs.push_back(lab);
  }
};
void Column::_bind_methods(){};
Column::Column(){};
Column::~Column(){};