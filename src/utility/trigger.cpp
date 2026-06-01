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
// ---- 绑定 Count 属性 ----
  ClassDB::bind_method(D_METHOD("get_count"), &Trigger::get_count);
  // 注意：Setter 函数在 D_METHOD 里的第二个参数是给形参起的名字，方便 GDScript 提示
  ClassDB::bind_method(D_METHOD("set_count", "count"), &Trigger::set_count);
  // 把 Getter 和 Setter 打包注册为 Godot 属性
  ClassDB::add_property("Trigger", PropertyInfo(Variant::INT, "count"), "set_count", "get_count");
// ---- 绑定 Fire Rate 属性 ----
  ClassDB::bind_method(D_METHOD("get_fire_rate"), &Trigger::get_fire_rate);
  ClassDB::bind_method(D_METHOD("set_fire_rate", "fire_rate"), &Trigger::set_fire_rate);
  // 打包注册为 Godot 属性
  ClassDB::add_property("Trigger", PropertyInfo(Variant::FLOAT, "fire_rate"), "set_fire_rate", "get_fire_rate");
}