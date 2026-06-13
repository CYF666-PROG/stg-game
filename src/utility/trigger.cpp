#include "trigger.hpp"

using namespace utility ;
using namespace godot;

int utility::Trigger::get_count(){
  return count;
}
double utility::Trigger::get_fire_rate(){
  return fire_rate;
}
Trigger::EmitterTYP utility::Trigger::get_emitter_typ(){
  return emitter_typ;
}
void Trigger::set_count(int count){
  this->count = count;
}
void utility::Trigger::set_fire_rate(double fire_rate){
  this->fire_rate = fire_rate;
}

void utility::Trigger::set_emitter_typ(EmitterTYP emitter_typ){
  this->emitter_typ = emitter_typ;
}

Trigger::Trigger(){}

Trigger::~Trigger(){}

void utility::Trigger::_bind_methods(){
  ClassDB::bind_method(D_METHOD("get_count"), &Trigger::get_count);
  ClassDB::bind_method(D_METHOD("set_count", "count"), &Trigger::set_count);
  ClassDB::add_property("Trigger", PropertyInfo(Variant::INT, "count"), "set_count", "get_count");

  ClassDB::bind_method(D_METHOD("get_fire_rate"), &Trigger::get_fire_rate);
  ClassDB::bind_method(D_METHOD("set_fire_rate", "fire_rate"), &Trigger::set_fire_rate);
  ClassDB::add_property("Trigger", PropertyInfo(Variant::FLOAT, "fire_rate"), "set_fire_rate", "get_fire_rate");
}

void Trigger::_ready() {
  // 清除所有碰撞层
  set_collision_layer(0);
  set_collision_mask(0);
  // 设置自身为第5层
  set_collision_layer_value(5, true);
  // 检测第3层
  set_collision_mask_value(3, true);
}