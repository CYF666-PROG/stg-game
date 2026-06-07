#include "text.hpp"

#include "godot_cpp/classes/control.hpp"
#include "godot_cpp/classes/wrapped.hpp"

#include <vector>


namespace ui {
class Column : public godot::Control{
  GDCLASS(Column, godot::Control)
  private:
  // 上一帧是否按下esc
  bool was_esc_pressed = false;
  bool was_up_pressed = false;
  bool was_dow_pressed = false;
  std::vector<ui::Text*> labs;
  int label_index = 0;
  public:
  void _physics_process(double delta) override;
  virtual void _ready() override ;
  static void _bind_methods();
  Column();
  ~Column();
};
}