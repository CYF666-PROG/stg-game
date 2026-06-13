#pragma once

#include "godot_cpp/classes/label.hpp"
#include "godot_cpp/classes/wrapped.hpp"

namespace ui {
class Text : public godot::Label{
  GDCLASS(Text, godot::Label)
  private:
  bool is_selected = false;
  int frame = 0;
  public:
  int godot_typ = 0;
  enum Typ {
    PLAY,
    QUIT,
  };
  Typ typ = PLAY;
  void play();
  void quit();
  void update_animation();
  void selected();
  void deselect();
  void set_typ(Typ typ);

  void set_godot_typ(int typ);
  int get_godot_typ();

  void _physics_process(double delta) override;
  virtual void _ready() override ;
  static void _bind_methods() ;
  Text(Typ typ);
  Text();
  ~Text();
};
}