#pragma once

#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/node.hpp>

namespace input
{
class KeyBoard : public godot::Node {
  GDCLASS(KeyBoard, godot::Node)

private:
  static KeyBoard* singleton ;
protected:
    static void _bind_methods();
public:
  bool is_right = false ;
  bool is_left = false ;
  bool is_up = false ;
  bool is_down = false ;
  bool is_shoot = false ;

  void update(double delta);
  static KeyBoard* get_singleton();


  KeyBoard();
  ~KeyBoard();
};
} // namespace input
