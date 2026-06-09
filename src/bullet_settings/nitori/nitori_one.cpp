#include "nitori_one.hpp"
#include "../../conf/bullet.hpp"

#include <godot_cpp/classes/resource_loader.hpp>

using namespace game::bullet_settings;
using namespace godot;

void NitoriOne::shoot(){
  if (frame % 240 == 0){
    now_count = 0;
    now_initial_direction_rad = Math::deg_to_rad(initial_direction_deg);
  }
  sector();
}

void game::bullet_settings::NitoriOne::sector(){
  if (now_count > count){
    return;
  }
  if (frame % frame_interval != 0){
    return;
  }
  // 加载尖头弹
  // 1. 获取资源加载器的单例
  ResourceLoader* loader = ResourceLoader::get_singleton();
  // 2. 直接加载资源并进行安全强转
  Ref<SpriteFrames> spr = loader->load("res://material/bullet/pointed/blue.tres");
  if (spr.is_null()){
    UtilityFunctions::print("not fond ","res://material/bullet/pointed/blue.tres");
  }
  double initial = now_initial_direction_rad; // 基准角度
  double rotation_offset = this->rotation_offset; // 贴图旋转偏移
  double bullet_rad = Math::deg_to_rad(bullet_deg); // 子弹间隔弧度
  double left_rad = 0; // 左边开始发射的角度
  double speed = this->speed; // 速度
  // 下方发射
  left_rad = initial - ((double(now_count)-1)/2) * bullet_rad;
  for (int i = 0; i < now_count; i++){
    double shoot_rad = left_rad + bullet_rad * i; // 射击角度
    auto linear_behavior = [speed, shoot_rad, rotation_offset](BulletPool::Bullet& b){
    b.velocity = Vector2(cos(shoot_rad),sin(shoot_rad)) * speed;
    b.rotation = rotation_offset + b.velocity.angle();
    };
    pool->spawn(
      get_global_position(),     
      linear_behavior,  
      spr,
      "normal",
      conf::bullet::pointed::radius,
      1,
      10,
      Vector2(1,1) * conf::bullet::pointed::scale
    );
  }
  // 左上发射
  left_rad = Math::deg_to_rad(double(-70)) - ((double(now_count)-1)/2) * bullet_rad;
  for (int i = 0; i < now_count; i++){
    double shoot_rad = left_rad + bullet_rad * i; // 射击角度
    auto linear_behavior = [speed, shoot_rad, rotation_offset](BulletPool::Bullet& b){
    b.velocity = Vector2(cos(shoot_rad),sin(shoot_rad)) * speed;
    b.rotation = rotation_offset + b.velocity.angle();
    };
    pool->spawn(
      get_global_position(),     
      linear_behavior,  
      spr,
      "normal",
      conf::bullet::pointed::radius,
      1,
      10,
      Vector2(1,1) * conf::bullet::pointed::scale
    );
  }
  // 右上发射
  left_rad = Math::deg_to_rad(double(-110)) - ((double(now_count)-1)/2) * bullet_rad;
  for (int i = 0; i < now_count; i++){
    double shoot_rad = left_rad + bullet_rad * i; // 射击角度
    auto linear_behavior = [speed, shoot_rad, rotation_offset](BulletPool::Bullet& b){
    b.velocity = Vector2(cos(shoot_rad),sin(shoot_rad)) * speed;
    b.rotation = rotation_offset + b.velocity.angle();
    };
    pool->spawn(
      get_global_position(),     
      linear_behavior,  
      spr,
      "normal",
      conf::bullet::pointed::radius,
      1,
      10,
      Vector2(1,1) * conf::bullet::pointed::scale
    );
  }
  now_count++;
  now_initial_direction_rad += Math::deg_to_rad(deflection_deg) ;
}

NitoriOne::NitoriOne() {
  now_count = 0;
  now_initial_direction_rad = Math::deg_to_rad(initial_direction_deg);
}

NitoriOne::~NitoriOne(){}
