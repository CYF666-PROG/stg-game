#include "nitori.hpp"
#include "../../../conf/bullet.hpp"
#include "../../../bullet_settings/nitori/nitori_zero.hpp"
#include "../../../bullet_settings/nitori/nitori_one.hpp"
#include "../../../bullet_settings/nitori/two.hpp"
#include "../../../bullet_settings/nitori/three.hpp"

#include "godot_cpp/classes/animated_sprite2d.hpp"
#include "godot_cpp/classes/rectangle_shape2d.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/resource_loader.hpp"
#include "godot_cpp/classes/sprite_frames.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/vector2.hpp"
#include <vector>

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
  level = 0;
  hp = 100;
  auto nitori_zero = memnew(game::bullet_settings::NitoriZero);
  add_child(nitori_zero);
  shoot_l = [nitori_zero](Boos* b){
    if(b->frame_status == 0) {
      UtilityFunctions::print("shoot_l re_set");
      nitori_zero->re_set();
    };
    if (nitori_zero->is_end()) {
      b->status = moveing;
    }
    b->frame_status++;
  };
  std::vector<Vector2> poss;
  poss.push_back(Vector2(577,154));
  poss.push_back(Vector2(428,124));
  poss.push_back(Vector2(435,166));
  move_l = [
    poss, // 位置数组
    pos_index = 0
  ](Boos* b) mutable {
    if (b->move(poss[pos_index])) {
      UtilityFunctions::print("move_l move end");
      pos_index = (pos_index+1) % poss.size();
      b->status = shooting;
      return;
    }
    b->frame_status++;
  };
  status = shooting;
}

void NiToRi::start_1(){
  level = 1;
  hp = 100;
  auto nitori_one = memnew(game::bullet_settings::NitoriOne);
  add_child(nitori_one);
  shoot_l = [nitori_one](Boos* b){
    if(b->frame_status == 0) {
      UtilityFunctions::print("shoot_l re_set");
      nitori_one->re_set();
    };
    if (nitori_one->is_end()) {
      b->status = moveing;
    }
    b->frame_status++;
  };
  std::vector<Vector2> poss;
  poss.push_back(Vector2(577,154));
  poss.push_back(Vector2(428,124));
  poss.push_back(Vector2(435,166));
  move_l = [
    poss, // 位置数组
    pos_index = 0
  ](Boos* b) mutable {
    if (b->move(poss[pos_index])) {
      UtilityFunctions::print("move_l move end");
      pos_index = (pos_index+1) % poss.size();
      b->status = shooting;
      return;
    }
    b->frame_status++;
  };
  status = shooting;
};

void NiToRi::start_2(){
  level = 2;
  hp = 100;
  auto nitori_two = memnew(game::bullet_settings::nitori::Two);
  add_child(nitori_two);
  status = moveing;
  shoot_l = [](Boos* b){
    if (b->frame_status > 60) {
      b->status = moveing;
    }
  };
  std::vector<Vector2> poss;
  poss.push_back(Vector2(478,202));
  poss.push_back(Vector2(693,177));
  move_l = [
    poss, // 位置数组
    pos_index = 0
  ](Boos* b) mutable {
    if (b->move(poss[pos_index])) {
      UtilityFunctions::print("move_l move end");
      pos_index = (pos_index+1) % poss.size();
      b->status = shooting;
      return;
    }
  };
};

void NiToRi::start_3(){
  level = 3;
  hp = 100;
  auto nitori_one = memnew(game::bullet_settings::NitoriOne);
  nitori_one->count = 20 ;
  nitori_one->deflection_deg = 0.2;
  nitori_one->disabled();
  add_child(nitori_one);
  status = moveing;
  shoot_l = [
    nitori_one,
    a = 1,               // 控制向左偏还是向右偏
    count = 0           // 当前将要发射几轮 初始是禁用所以要设为-1
  ](Boos* b) mutable{
    if(b->frame_status == 0){
      count = 0;
    }
    if (nitori_one->is_end()) {
      count++;
      if (count > 4) {
        b->status = moveing;
        return;
      }
      if (count % 2 == 0) {
        nitori_one->deflection_deg *= -1;
      }
      nitori_one->re_set();
    }
  };
  std::vector<Vector2> poss;
  poss.push_back(Vector2(478,202));
  poss.push_back(Vector2(693,177));
  move_l = [
    poss, // 位置数组
    pos_index = 0
  ](Boos* b) mutable {
    if (b->move(poss[pos_index])) {
      UtilityFunctions::print("move_l move end");
      pos_index = (pos_index+1) % poss.size();
      b->status = shooting;
      return;
    }
  };
};

void NiToRi::start_4(){
  level = 3;
  hp = 100;
  auto nitori_two = memnew(game::bullet_settings::nitori::Three);
  add_child(nitori_two);
  status = moveing;
  shoot_l = [](Boos* b){
    if (b->frame_status > 60) {
      b->status = moveing;
    }
  };
  std::vector<Vector2> poss;
  poss.push_back(Vector2(478,202));
  poss.push_back(Vector2(693,177));
  move_l = [
    poss, // 位置数组
    pos_index = 0
  ](Boos* b) mutable {
    if (b->move(poss[pos_index])) {
      UtilityFunctions::print("move_l move end");
      pos_index = (pos_index+1) % poss.size();
      b->status = shooting;
      return;
    }
  };
};