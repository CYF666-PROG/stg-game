#include "ui_manager.hpp"
#include "bullet_pool.hpp"
#include "effect_manager.hpp"
#include "godot_cpp/classes/animated_sprite2d.hpp"
#include "godot_cpp/classes/label.hpp"
#include "godot_cpp/classes/node2d.hpp"
#include "godot_cpp/classes/sprite2d.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "godot_cpp/variant/vector2.hpp"
#include "levelmanager.hpp"
#include "player/player.hpp"
#include "../conf/bullet.hpp"

#include "godot_cpp/classes/atlas_texture.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/resource_loader.hpp"
#include "godot_cpp/variant/color.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include <godot_cpp/classes/scene_tree.hpp>
#include <cstdint>

using namespace godot;
using namespace game;
using namespace conf::player;

UiManager* UiManager::the_ui_manager = nullptr;

void UiManager::check_player(){
  if (!load_status_ui()) return;
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

bool UiManager::load_menu(){
  // 加载ui文字
  menu_label[0] = get_node<Label>("/root/play/menu/continue");
  menu_label[1] = get_node<Label>("/root/play/menu/exit");
  menu_label[2] = get_node<Label>("/root/play/menu/retry");
  for (auto it : menu_label) {
    if (!it) {
      UtilityFunctions::print("UiManager::load_menu erro");
      return false;
    }
  }
  // 加载menu节点
  menu_node = get_node<Node2D>("/root/play/menu");
  if (!menu_node) {
    UtilityFunctions::print("/root/play/menu load erro");
    return false;
  }
  return true;
}

void UiManager::pause(){
  auto pool = game::BulletPool::get_pool();
  if (!pool) {
    UtilityFunctions::print("UiManager::pause get_pool erro");
    return;
  }
  auto eff = game::EffectManager::get_singleton();
  if (!eff) {
    UtilityFunctions::print("UiManager::pause get_singleton erro");
    return;
  }
  auto play = get_node<Node2D>("/root/play");
  if (!play) {
    return;
  }
  auto pause = get_node<Label>("/root/play/menu/pause");
  auto dead = get_node<Label>("/root/play/menu/dead");
  if (!pause || !dead) {
    return;
  }
  pause->set_visible(true);
  dead->set_visible(false);
  // 暂停场景根节点
  play->set_process_mode(godot::Node::PROCESS_MODE_DISABLED);
  // 暂停特效 弹幕池
  pool->set_physics_process(false);
  eff->set_physics_process(false);
  status_typ = PAUSE;
  if (!load_menu()) {
    UtilityFunctions::print("UiManager::pause load_menu erro");
  }
};

void UiManager::play(){
  auto pool = game::BulletPool::get_pool();
  if (!pool) {
    UtilityFunctions::print("UiManager::play get_pool erro");
    return;
  }
  auto eff = game::EffectManager::get_singleton();
  if (!eff) {
    UtilityFunctions::print("UiManager::play get_singleton erro");
    return;
  }
  auto play = get_node<Node2D>("/root/play");
  if (!play) {
    return;
  }
  status_typ = PLAYING;
  menu_node->set_global_position(Vector2(-936,0));
  // 恢复根节点
  play->set_process_mode(godot::Node::PROCESS_MODE_INHERIT);
  // 恢复特效 弹幕池
  pool->set_physics_process(true);
  eff->set_physics_process(true);
}


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
  if(status_typ == PAUSE){
    update_menu();
  }
  // 只在暂停时处理点击事件
  if (status_typ == PAUSE && keyboard->is_determine) {
    if (menu_label_index == 0) {
      play();
    }else if (menu_label_index == 1) {
      quit();
    }else if (menu_label_index == 2) {
      restart();
    }
  }
  was_esc_pressed = keyboard->is_esc;
  ++frame;
}

void UiManager::update_menu(){
  auto keyboard = input::KeyBoard::get_singleton();
  if (!keyboard) {
    UtilityFunctions::print("UiManager::update_menu keyboard erro");
    return;
  }
  if (!menu_node) {
    UtilityFunctions::print("UiManager::update_menu menu_node not foud");
    return;
  }
  // 处理从左向右移动
  auto pos = menu_node->get_global_position();
  if(pos.x < 93.6){
    pos.x += 100;
    menu_node->set_global_position(pos);
  }
  if (Math::abs(pos.x) < 150) {
    menu_node->set_global_position(Vector2(0,0));
  }
  // 更新选中的索引
  double speed_multiplier = 0.05; 
  float factor = (std::sin(frame * speed_multiplier) + 1.0f) * 0.5f;
  godot::Color normal_color = godot::Color(1.0f, 1.0f, 1.0f); // 默认白色
  godot::Color red_color = godot::Color(1.0f, 0.0f, 0.0f); // 警示红色
  godot::Color current_color = normal_color.lerp(red_color, factor);

  if (!was_dow_pressed && keyboard->is_down) {
    menu_label[menu_label_index]->add_theme_color_override("font_color", normal_color);
    menu_label_index = (menu_label_index+1)%3;
  }else if (!was_up_pressed && keyboard->is_up) {
    menu_label[menu_label_index]->add_theme_color_override("font_color", normal_color);
    menu_label_index = (menu_label_index+2)%3;
  }
  // 6. 每一帧实时更新 Label 的颜色
  menu_label[menu_label_index]->add_theme_color_override("font_color", current_color);

  was_dow_pressed = keyboard->is_down;
  was_up_pressed = keyboard->is_up;
}

void UiManager::dead(){
  auto pool = game::BulletPool::get_pool();
  if (!pool) {
    UtilityFunctions::print("UiManager::pause get_pool erro");
    return;
  }
  auto eff = game::EffectManager::get_singleton();
  if (!eff) {
    UtilityFunctions::print("UiManager::pause get_singleton erro");
    return;
  }
  auto play = get_node<Node2D>("/root/play");
  if (!play) {
    return;
  }
  auto pause = get_node<Label>("/root/play/menu/pause");
  auto dead = get_node<Label>("/root/play/menu/dead");
  if (!pause || !dead) {
    return;
  }
  pause->set_visible(false);
  dead->set_visible(true);
  // 暂停场景根节点
  play->set_process_mode(godot::Node::PROCESS_MODE_DISABLED);
  // 暂停特效 弹幕池
  pool->set_physics_process(false);
  eff->set_physics_process(false);
  status_typ = PAUSE;
  if (!load_menu()) {
    UtilityFunctions::print("UiManager::pause load_menu erro");
  }
}

void UiManager::quit(){
  status_typ = TITLE;
  // 1. 获取当前节点的 SceneTree
  SceneTree *tree = get_tree();
  if (tree) {
    // 2. 调用切换场景方法（传入 Godot 虚拟路径）
    Error err = tree->change_scene_to_file("res://scene/title.tscn");
    if (err != OK) {
      UtilityFunctions::printerr( err);
    }
  }
};

void UiManager::restart(){
  auto level = game::LevelManager::get_singleton();
  if (!level) {
    UtilityFunctions::print("restart LevelManager not foud");
    return;
  }
  // 重新加载场景
  status_typ = PLAYING;
  SceneTree *tree = get_tree(); 
  if (tree) {
    Error err = tree->change_scene_to_file("res://scene/title.tscn");
    if (err != OK) {
      UtilityFunctions::printerr( err);
    }
  }

  tree = get_tree(); 
  if (tree) {
    Error err = tree->change_scene_to_file("res://scene/play.tscn");
    if (err != OK) {
      UtilityFunctions::printerr( err);
    }
  }
  // 重新在加载
  load_status_ui();
  load_menu();
};

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
  menu_label.resize(3,nullptr);
};