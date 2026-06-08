#include "sunflower.hpp"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/circle_shape2d.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/atlas_texture.hpp>
#include <godot_cpp/classes/sprite2d.hpp>

using namespace godot ;
using namespace game::bullet_settings ;

void Sunflower::shoot(){
  int cycle = 120 ; // 每隔多少帧一轮
  if (frame % cycle == 0){
    outside();
  }
  if ((frame+90) % cycle == 0){
    inside();
  }
}

void game::bullet_settings::Sunflower::inside(){
  
  double direction_count = 24; // 发射方向个数
  double count = 5; // 发射轮数
  double degree = 10; // 每轮间隔多少度
  double max = 6 ;// 最大速度
  double min = 1 ;// 最小速度
  double plus = 20 ; // 加速帧数
  double stop = 10; // 停顿帧数
  double end = 3 ; // 最终速度

  double rotation_offset = this->rotation_offset;
  double benchmark_rad = 0; // 基准方向
  for (int i = 0; i < count; i++){
    double rad = 2.0*Math_PI/direction_count; // 一轮子弹角度间隔
    for (int j = 1; j <= direction_count; j++){
      double spead = min + (double(i)/count)*(max-min);
      double shoot_rad = 0 ; //发射的方向角
      shoot_rad = benchmark_rad + j * rad;

      auto linear_behavior = [rad,j,spead,shoot_rad,min,rotation_offset,end,stop,plus](BulletPool::Bullet& b){
        if(b.lifetime < plus){
          b.velocity = Vector2(cos(shoot_rad), sin(shoot_rad)) * spead;
        }else if(b.lifetime > stop + plus) {
          b.velocity = Vector2(cos(shoot_rad), sin(shoot_rad)) * end;
        }else{
          b.velocity = Vector2(0,0);
        }
        if (!b.velocity.is_zero_approx()){
          b.rotation = rotation_offset + b.velocity.angle();
        }
      };

      pool->spawn(
        get_global_position(),     
        linear_behavior,  
        to_launch_texture,  
        "normal",
        radius,
        1,
        10,
        zoom
      );
    }
    benchmark_rad += Math::deg_to_rad(degree);
  }
}

void game::bullet_settings::Sunflower::outside(){
  
  double direction_count = 14; // 发射方向个数
  double count = 10; // 发射轮数
  double degree = 2; // 每轮间隔多少度
  double max = 6 ;// 最大速度
  double min = 1 ;// 最小速度
  double plus = 20 ; // 加速帧数
  double stop = 10; // 停顿帧数
  double end = 3 ; // 最终速度

  double rotation_offset = this->rotation_offset;
  double benchmark_rad = 0; // 基准方向
  for (int i = 0; i < count; i++){
    double rad = 2.0*Math_PI/direction_count; // 一轮子弹角度间隔
    for (int j = 1; j <= direction_count; j++){
      double spead = min + (double(i)/count)*(max-min);
      double shoot_rad = 0 ; //发射的方向角
      shoot_rad = benchmark_rad + j * rad;

      auto linear_behavior = [rad,j,spead,shoot_rad,min,rotation_offset,end,stop,plus](BulletPool::Bullet& b){
        if(b.lifetime < plus){
          b.velocity = Vector2(cos(shoot_rad), sin(shoot_rad)) * spead;
        }else if(b.lifetime > stop + plus) {
          b.velocity = Vector2(cos(shoot_rad), sin(shoot_rad)) * end;
        }else{
          b.velocity = Vector2(0,0);
        }
        if (!b.velocity.is_zero_approx()){
          b.rotation = rotation_offset + b.velocity.angle();
        }
      };

      pool->spawn(
        get_global_position(),     
        linear_behavior,  
        to_launch_texture,  
        "normal",
        radius,
        1,
        10,
        zoom
      );
    }
    benchmark_rad += Math::deg_to_rad(degree);
  }
}

Sunflower::Sunflower() {}

Sunflower::~Sunflower(){}