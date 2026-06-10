#include "nitori.hpp"
#include "../../../conf/bullet.hpp"
#include "../../../bullet_settings/nitori/nitori_zero.hpp"

#include "godot_cpp/classes/animated_sprite2d.hpp"
#include "godot_cpp/classes/rectangle_shape2d.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/resource_loader.hpp"
#include "godot_cpp/classes/sprite_frames.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/vector2.hpp"

using namespace game::boos;
using namespace godot;

void NiToRi::_ready(){
  //  调用父类的准备
  Boos::_ready();
  // 加载NiToRi贴图
  String path = String(conf::boos::nitori::path.c_str());
  double scale = conf::boos::nitori::scale;
  // 获取资源加载器的单例
  ResourceLoader* loader = ResourceLoader::get_singleton();
  Ref<SpriteFrames> spr = loader->load(path);
  if (spr.is_null()){
    UtilityFunctions::print("not fond ",path);
  }
  anima = memnew(AnimatedSprite2D);
  anima->set_sprite_frames(spr);
  anima->set_scale(Vector2(1,1) * scale);
  add_child(anima);
  // 设置碰撞
  // 1. 创建 CollisionShape2D 子节点
  CollisionShape2D* collision_shape = memnew(CollisionShape2D);
  // 2. 创建具体的几何形状资源（比如一个 20x20 的矩形）
  Ref<RectangleShape2D> rect_shape;
  rect_shape.instantiate();
  rect_shape->set_size(godot::Vector2(64, 38)); // 设置宽高
  // 3. 将形状资源赋值给碰撞节点
  collision_shape->set_shape(rect_shape);
  // 4. 将碰撞节点添加为 Area2D 的子节点
  add_child(collision_shape);
}

void NiToRi::start_0(){
  auto f = memnew(game::bullet_settings::NitoriZero);
  add_child(f);
}