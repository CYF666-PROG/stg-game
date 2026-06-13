#pragma once

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/tween.hpp>

namespace ui {

class CardName : public godot::Control {
    GDCLASS(CardName, godot::Control);

private:
  godot::Label* label_node = nullptr;
  godot::String card_text = "";

  const godot::Vector2 SCREEN_RIGHT_BOTTOM = godot::Vector2(936, 1044); // 屏幕右下角
  const godot::Vector2 SCREEN_LEFT_TOP     = godot::Vector2(72, 36);       // 屏幕左上角
  
  double margin = 10.0;  // 边距
  double duration = 2.0; // 动画时间

protected:
    static void _bind_methods();

public:
  CardName();
  ~CardName();
  void _ready() override;

  void setup_and_play(const godot::String &p_text);
};

} // namespace godot