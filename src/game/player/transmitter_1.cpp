#include "transmitter_1.hpp"
#include "../../conf/bullet.hpp"

#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/sprite2d.hpp>


using namespace godot;

using namespace game::player;

void game::player::Transmitter_1::_ready(){
  // 获取子弹贴图
  // 1. 获取资源加载器的单例
  godot::ResourceLoader* loader = godot::ResourceLoader::get_singleton();
  // 2. 直接加载资源并进行安全强转
  texture = loader->load(bullet_path);
  // 3. 检查是否加载成功
  if (!texture.is_valid()) {
    godot::UtilityFunctions::print(bullet_path, " load erro");
    return;
  }
  // 设置阴阳玉贴图
  Ref<godot::AtlasTexture> ying_yang_texture = loader->load(path);
  if (ying_yang_texture.is_valid()){
    spr = memnew(Sprite2D);
    spr->set_texture(ying_yang_texture);
    spr->set_scale(Vector2(1,1) * scale);
    add_child(spr);
  }else 
    UtilityFunctions::print(path, " load erro");
  // 设置弹幕池 键盘输入
  pool = game::BulletPool::get_pool();
  keyboard = input::KeyBoard::get_singleton();
}

void Transmitter_1::frame_do(double delta){
  //更新阴阳玉旋转
  spr->set_rotation(spr->get_rotation() + 0.2);
  if (!keyboard){
    UtilityFunctions::print("Transmitter_1: keyboard not found");
    return;
  }
  if(keyboard->is_shoot) shoot();
}

void Transmitter_1::shoot(){
  if (!pool){
    UtilityFunctions::print("Transmitter_1: pool not found");
    pool = game::BulletPool::get_pool();
    return;
  }
  auto linear_behavior = [](BulletPool::Bullet& b){
    b.rotation = conf::bullet::player_1::rotation_offset;
    b.velocity = Vector2(0, -1) * (80 + UtilityFunctions::randf_range(-5,5));
  };
  pool->spawn(
    get_global_position() + Vector2(-10,0),
    linear_behavior,
    0,
    texture,
    "normal",
    bullet_radius,
    Vector2(1,1) * bullet_scale,
    bullet_anchor
  );
  pool->spawn(
    get_global_position() + Vector2(10,0),
    linear_behavior,
    0,
    texture,
    "normal",
    bullet_radius,
    Vector2(1,1) * bullet_scale,
    bullet_anchor
  );
}

game::player::Transmitter_1::Transmitter_1(orb_Typ orb_typ, bullet_Typ bullet_typ){
  if (orb_typ == Red){
    path = String(conf::player::red_yin_yang_orb::path.c_str());
    scale = conf::player::red_yin_yang_orb::scale;
  }else if (orb_typ == Pink){
    path = String(conf::player::pink_yin_yang_orb::path.c_str());
    scale = conf::player::pink_yin_yang_orb::scale;
  }else{
    path = String(conf::player::blue_yin_yang_orb::path.c_str());
    scale = conf::player::blue_yin_yang_orb::scale;
  }
  if (bullet_typ == fast_bullet_1){
    bullet_path = String(conf::bullet::player_1::path.c_str());
    bullet_anchor = Vector2(conf::bullet::player_1::anchor_x,
                            conf::bullet::player_1::anchor_y);
    bullet_rotation_offset = conf::bullet::player_1::rotation_offset;
    bullet_scale = conf::bullet::player_1::scale;
  }else if (bullet_typ == fast_bullet_2){
    bullet_path = String(conf::bullet::player_2::path.c_str());
    bullet_anchor = Vector2(conf::bullet::player_2::anchor_x,
                            conf::bullet::player_2::anchor_y);
    bullet_rotation_offset = conf::bullet::player_2::rotation_offset;
    bullet_scale = conf::bullet::player_2::scale;
  }else{
    bullet_path = String(conf::bullet::player_2::path.c_str());
    bullet_anchor = Vector2(conf::bullet::player_2::anchor_x,
                            conf::bullet::player_2::anchor_y);
    bullet_rotation_offset = conf::bullet::player_2::rotation_offset;
    bullet_scale = conf::bullet::player_2::scale;
  }
  
}

Transmitter_1::~Transmitter_1()
{
}

void game::player::Transmitter_1::_bind_methods()
{
}
