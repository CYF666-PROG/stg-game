#include "keyboard.hpp"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>

using namespace godot;
using namespace input;

KeyBoard* KeyBoard::singleton = nullptr;


void KeyBoard::update(double delta){
  Input *input = Input::get_singleton();
  if (!input) {
    return;
  };
  this->is_left = input->is_key_pressed(KEY_LEFT);
  this->is_right = input->is_key_pressed(KEY_RIGHT);
  this->is_up = input->is_key_pressed(KEY_UP);
  this->is_down = input->is_key_pressed(KEY_DOWN);
  this->is_shoot = input->is_key_pressed(KEY_Z);
}

KeyBoard *input::KeyBoard::get_singleton(){
  return singleton;
}

void KeyBoard::_bind_methods(){
}

KeyBoard::KeyBoard() {
  if (KeyBoard::get_singleton() != nullptr){
    return ;
  }
  UtilityFunctions::print("KeyBoard add", this);
  KeyBoard::singleton = this ;
}
KeyBoard::~KeyBoard(){
  KeyBoard::singleton = nullptr;
}