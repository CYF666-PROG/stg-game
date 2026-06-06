#include "ui_manager.hpp"
#include "godot_cpp/classes/animated_sprite2d.hpp"
#include "godot_cpp/classes/sprite2d.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "godot_cpp/variant/vector2.hpp"
#include "player/player.hpp"
#include "../conf/bullet.hpp"

#include "godot_cpp/classes/atlas_texture.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/resource_loader.hpp"
#include "godot_cpp/variant/color.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include <cstdint>

using namespace godot;
using namespace game;
using namespace conf::player;

UiManager* UiManager::the_ui_manager = nullptr;

void UiManager::check_player(){
  if (!load_status_ui()) return;
  if (!load_heart_card()) return;
  // 先设为空心
  for (int i = 0; i < hearts.size(); ++i) {
    if (hearts[i] == nullptr) continue;
    hearts[i]->set_texture(hearts_tex[0]);
  }
  // 先设为空星
  for (int i = 0; i < stars.size(); ++i) {
    if (stars[i] == nullptr) continue;
    stars[i]->set_texture(star_tex[0]);
  }
  auto hp = player->hp;
  // 在根据残机修改
  for (int i = 0; i < hearts.size(); ++i) {
    if (hp - 5 >= 0) {
      hearts[i]->set_texture(hearts_tex[5]);
    }else if (hp - 4 >= 0) {
      hearts[i]->set_texture(hearts_tex[4]);
    }else if (hp - 3 >= 0) {
      hearts[i]->set_texture(hearts_tex[3]);
    }else if (hp - 2 >= 0) {
      hearts[i]->set_texture(hearts_tex[2]);
    }else if (hp - 1 >= 0){
      hearts[i]->set_texture(hearts_tex[1]);
    }else {
      hearts[i]->set_texture(hearts_tex[0]);
    }
    hp -= 5;
  }
  auto card = player->card;
  // 在根据符卡数修改
  for (int i = 0; i < stars.size(); ++i) {
    if (card - 5 >= 0) {
      stars[i]->set_texture(star_tex[5]);
    }else if (card - 4 >= 0) {
      stars[i]->set_texture(star_tex[4]);
    }else if (card - 3 >= 0) {
      stars[i]->set_texture(star_tex[3]);
    }else if (card - 2 >= 0) {
      stars[i]->set_texture(star_tex[2]);
    }else if (card - 1 >= 0){
      stars[i]->set_texture(star_tex[1]);
    }else {
      stars[i]->set_texture(star_tex[0]);
    }
    card -= 5;
  }
}

bool UiManager::load_status_ui(){

  player = get_node<Player>("/root/play/Player");
  if (!player) {
    UtilityFunctions::print("UiManager::check_player not found /root/play/Player");
    return false;
  }
  status_ui = get_node<ColorRect>("/root/play/StatusUi");
  if (!status_ui) {
    UtilityFunctions::print("UiManager::check_player not found /root/play/StatusUi");
    return false;
  }
  return true;
}

bool UiManager::load_heart_card(){
  // 1. 获取资源加载器的单例
  godot::ResourceLoader* loader = godot::ResourceLoader::get_singleton();
  // 获取心贴图
  hearts_tex[0] = loader->load(String(heart::path_0.c_str()));
  hearts_tex[1] = loader->load(String(heart::path_1.c_str()));
  hearts_tex[2] = loader->load(String(heart::path_2.c_str()));
  hearts_tex[3] = loader->load(String(heart::path_3.c_str()));
  hearts_tex[4] = loader->load(String(heart::path_4.c_str()));
  hearts_tex[5] = loader->load(String(heart::path_full.c_str()));
  /// 检查贴图是否有效
  for (const auto it : hearts_tex) {
    if (!it.is_valid()) {
      UtilityFunctions::print("UiManager::check_player load hearts_tex erro");
      return false;
    }
  }
  // 获取心
  for (int i = 0;i < 8 ; ++i) {
    auto path = UtilityFunctions::str("/root/play/StatusUi/heart/",i);
    auto heart = get_node<Sprite2D>(path);
    if (!heart) {
      UtilityFunctions::print(path," not foud");
      return false;
    }
    hearts[i] = heart;
  }
  // 获取星贴图
  star_tex[0] = loader->load(String(star::path_0.c_str()));
  star_tex[1] = loader->load(String(star::path_1.c_str()));
  star_tex[2] = loader->load(String(star::path_2.c_str()));
  star_tex[3] = loader->load(String(star::path_3.c_str()));
  star_tex[4] = loader->load(String(star::path_4.c_str()));
  star_tex[5] = loader->load(String(star::path_full.c_str()));
  /// 检查贴图是否有效
  for (const auto it : star_tex) {
    if (!it.is_valid()) {
      UtilityFunctions::print("UiManager::check_player load star_tex erro");
      return false;
    }
  }
  // 获取星
  for (int i = 0;i < 8 ; ++i) {
    auto path = UtilityFunctions::str("/root/play/StatusUi/card/",i);
    auto star = get_node<Sprite2D>(path);
    if (!star) {
      UtilityFunctions::print(path," not foud");
      return false;
    }
    stars[i] = star;
  }
  return true;
}

void UiManager::pause(){
};

void UiManager::play(){}


void UiManager::_physics_process(double delta){
  auto keyboard = input::KeyBoard::get_singleton();
  if (!keyboard) return;
  // 切换游戏状态
  // 暂停游戏
  if (status_typ == PLAYING && !was_esc_pressed && keyboard->is_esc) {
    pause();
  }else if (status_typ == PAUSE && !was_esc_pressed && keyboard->is_esc) {
    play();
  }

  was_esc_pressed = keyboard->is_esc;
}

void UiManager::_ready(){
  the_ui_manager = this;
};

void UiManager::_bind_methods(){}

UiManager* UiManager::get_ui_manager(){return the_ui_manager;}

UiManager::~UiManager(){}

UiManager::UiManager(){
  hearts.resize(8, nullptr);
  hearts_tex.resize(6, nullptr);
  stars.resize(8,nullptr);
  star_tex.resize(6,nullptr);
};