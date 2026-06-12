#include "levelmanager.hpp"
#include "../utility/move.hpp"
#include "../utility/move/circle.hpp"
#include "enemy/boos.hpp"
#include "enemy/boos/nitori.hpp"
#include "enemy/imp.hpp"
#include "enemy/minion.hpp"
#include "enemy/rotate.hpp"
#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/variant/string.hpp"

#include <cstdint>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/path_follow2d.hpp>


using namespace game;
using namespace godot;

LevelManager* LevelManager::singleton = nullptr;

void game::LevelManager::_physics_process(double delta){
  /// 检查是否在编辑器
  if (godot::Engine::get_singleton()->is_editor_hint()){
    return;
  }
  // 检查关卡
  if (level_frame >= level_2_time * 60) {
    level_2();
    level_2_time = 99999999;
  }else if (level_frame >= level_3_time * 60) {
    level_3();
    level_3_time = 99999999;
  }

  
  auto it_end = level_timeline.lower_bound(double(level_frame)/60);
  for (auto it = level_timeline.begin(); it != it_end;) {
    // 先处理boos
    if (it->second.typ == NITORI) {
      auto *current_scene_root = get_tree()->get_current_scene();
      if (!current_scene_root) {
        UtilityFunctions::print("LevelManager::_physics_process scene_root not foud");
      }
      auto nitori = memnew(game::boos::NiToRi);
      nitori->set_level(it->second.level);
      current_scene_root->add_child(nitori);
      UtilityFunctions::print("make nitori");
    }else {
      auto* pf = memnew(godot::PathFollow2D);
      // 先设置禁用旋转，再添加
      pf->set_rotates(false);
      pf->set_loop(false);
      it->second.path->add_child(pf);
      auto min = get_minion(it->second.typ,it->second.coler);
      min->path_follow = pf;
      min->hp = it->second.hp;
      pf->add_child(min);
    }

    it = level_timeline.erase(it);
  }
  if(!is_pause) level_frame += 1;
}

void LevelManager::pause(){is_pause = true;}
void LevelManager::start(){is_pause = false;}

void game::LevelManager::_bind_methods(){}

void game::LevelManager::_ready(){
  singleton = this;
  godot::UtilityFunctions::print("LevelManager _ready");
  // 1. 获取场景树
  godot::SceneTree* tree = get_tree();
  if (!tree) {
    godot::UtilityFunctions::print("错误：无法获取 SceneTree");
    return;
  }
  // 2. 获取当前活动的主场景根节点
  // 无论当前关卡叫什么名字，它返回的都是当前关卡的最顶层节点
  current_scene = tree->get_current_scene();
  if (!current_scene) {
    godot::UtilityFunctions::print("错误：当前场景未加载完成或不存在");
    return;
  }
  level_1();
}

void LevelManager::level_1(){
  now_level = 1;
  for (double time = 2.5; time <= 8; time += 0.5) {
    double deat = UtilityFunctions::randf_range(-0.5, 0.5);
    make_enemy(time + deat, IMP,blue, get_path2d("level1/left_1"), 20);
    deat = UtilityFunctions::randf_range(-0.5, 0.5);
    make_enemy(time + deat, IMP,red, get_path2d("level1/left_2"), 20);
    deat = UtilityFunctions::randf_range(-0.5, 0.5);
    make_enemy(time + deat, IMP,yellow, get_path2d("level1/left_3"),20);
  }
  make_enemy(3, IMP,red, get_path2d("level1/right_up_1"),20);
  make_enemy(3.2, IMP,blue, get_path2d("level1/right_up_2"),20);
  make_enemy(3.4, IMP,blue, get_path2d("level1/right_up_3"),20);
}

void LevelManager::level_2(){
  now_level = 2;
  // level_2开始的秒
  double t = level_2_time;

  make_enemy(t + 1, BIG_butterfly,red, get_path2d("level2/left_up"),100);
  make_enemy(t + 1, BIG_butterfly,red, get_path2d("level2/right_up"),100);

  make_enemy(t + 4, IMP,red, get_path2d("level2/left_up_2"),20);
  make_enemy(t + 4.2, IMP,blue, get_path2d("level2/left_up"),20);
  make_enemy(t + 5, IMP,blue, get_path2d("level2/right_up"),20);
  make_enemy(t + 5.2, IMP,red, get_path2d("level2/right_up_2"),20);


  make_enemy(t + 7, IMP,red, get_path2d("level2/left_up_2"),20);
  make_enemy(t + 7.2, IMP,blue, get_path2d("level2/left_up"),20);
  make_enemy(t + 9, IMP,blue, get_path2d("level2/right_up"),20);
  make_enemy(t + 9.2, IMP,red, get_path2d("level2/right_up_2"),20);

  for (double i = 9; i <= 11; i += 0.5) {
    double deat = UtilityFunctions::randf_range(-0.2, 0.2);
    make_enemy(t + deat + i, IMP,blue, get_path2d("level2/right"), 20);
    deat = UtilityFunctions::randf_range(-0.2, 0.2);
    make_enemy(t + deat + i, IMP,red, get_path2d("level2/right_2"), 20);
    deat = UtilityFunctions::randf_range(-0.2, 0.2);
    make_enemy(t + deat + i, IMP,yellow, get_path2d("level2/right_3"),20);
    deat = UtilityFunctions::randf_range(-0.2, 0.2);
    make_enemy(t + deat + i, IMP,yellow, get_path2d("level2/right_4"),20);
  }
  for (double i = 11; i <= 13; i += 0.5) {
    double deat = UtilityFunctions::randf_range(-0.2, 0.2);
    make_enemy(t + deat + i, IMP,blue, get_path2d("level2/left"), 20);
    deat = UtilityFunctions::randf_range(-0.2, 0.2);
    make_enemy(t + deat + i, IMP,red, get_path2d("level2/left_2"), 20);
    deat = UtilityFunctions::randf_range(-0.2, 0.2);
    make_enemy(t + deat + i, IMP,yellow, get_path2d("level2/left_3"),20);
    deat = UtilityFunctions::randf_range(-0.2, 0.2);
    make_enemy(t + deat + i, IMP,yellow, get_path2d("level2/left_4"),20);
  }
  make_enemy(t + 10, IMP,red, get_path2d("level2/left_up_2"),20);
  make_enemy(t + 10.2, IMP,blue, get_path2d("level2/left_up"),20);
  // 左右交替向上出怪
  for (int i = 0; i < 3; i++) {
    double b = i * 4.666;   // 三轮之间所隔时间
    for (double i = 17+b; i <= 19+b; i += 0.5) {
      String path ;
      int a = UtilityFunctions::randi_range(1, 4);
      if (a == 1) path = "level2/left_down";
      if (a == 2) path = "level2/left_down_2";
      if (a == 3) path = "level2/left_down_3";
      if (a == 4) path = "level2/left_down_4";
      Color color;
      a = UtilityFunctions::randi_range(1, 3);
      if (a == 1) color = blue; 
      if (a == 2) color = red; 
      if (a == 3) color = yellow;
      make_enemy(i + t, IMP,color, get_path2d(path),20);
    }

    for (double i = 19+b; i <= 21+b; i += 0.5) {
      String path ;
      int a = UtilityFunctions::randi_range(1, 4);
      if (a == 1) path = "level2/right_down";
      if (a == 2) path = "level2/right_down_2";
      if (a == 3) path = "level2/right_down_3";
      if (a == 4) path = "level2/right_down_4";
      Color color;
      a = UtilityFunctions::randi_range(1, 3);
      if (a == 1) color = blue; 
      if (a == 2) color = red; 
      if (a == 3) color = yellow;
      make_enemy(i + t, IMP,color, get_path2d(path),20);
    }
  }
  make_enemy(t + 38, BIG_butterfly,red, get_path2d("level2/left_up"),100);
  make_enemy(t + 38, BIG_butterfly,red, get_path2d("level2/right_up"),100);
  for (double i = 50; i <= 55; i += 0.2) {
    String path ;
    int a = UtilityFunctions::randi_range(1, 4);
    if (a == 1) path = "level2/left_5";
    if (a == 2) path = "level2/left_6";
    if (a == 3) path = "level2/left_7";
    if (a == 4) path = "level2/left_8";
    Color color;
    a = UtilityFunctions::randi_range(1, 3);
    if (a == 1) color = blue; 
    if (a == 2) color = red; 
    if (a == 3) color = yellow;
    make_enemy(i + t, IMP,color, get_path2d(path),20);

    a = UtilityFunctions::randi_range(1, 4);
    if (a == 1) path = "level2/right_5";
    if (a == 2) path = "level2/right_6";
    if (a == 3) path = "level2/right_7";
    if (a == 4) path = "level2/right_8";
    a = UtilityFunctions::randi_range(1, 3);
    if (a == 1) color = blue; 
    if (a == 2) color = red; 
    if (a == 3) color = yellow;
    make_enemy(i + t, IMP,color, get_path2d(path),20);

  }
  make_boos(t+57,NITORI,0);
}

void LevelManager::level_3(){
  now_level = 3;
  double t = level_3_time;
  for (int i = 0;i < 2 ; i++) {
    double t = level_3_time + double(i) * 6 ;
    make_enemy(t, BIG_butterfly,red, get_path2d("level3/left_up"),100);
    for (double j = t;j < t + 1.5 ; j += 0.2) {
      String path ;
      int a = UtilityFunctions::randi_range(1, 8);
      if (a == 1) path = "level3/left";
      if (a == 2) path = "level3/left_2";
      if (a == 3) path = "level3/left_3";
      if (a == 4) path = "level3/left_4";
      if (a == 5) path = "level3/right";
      if (a == 6) path = "level3/right_2";
      if (a == 7) path = "level3/right_3";
      if (a == 8) path = "level3/right_4";
      Color color;
      a = UtilityFunctions::randi_range(1, 3);
      if (a == 1) color = blue; 
      if (a == 2) color = red; 
      if (a == 3) color = yellow;
      make_enemy(j, IMP,color, get_path2d(path),20);
    }
    make_enemy(t + 3, BIG_butterfly,red, get_path2d("level3/right_up"),100);
    for (double j = t+3;j < t + 1.5 + 3 ; j += 0.2) {
      String path ;
      int a = UtilityFunctions::randi_range(1, 8);
      if (a == 1) path = "level3/left";
      if (a == 2) path = "level3/left_2";
      if (a == 3) path = "level3/left_3";
      if (a == 4) path = "level3/left_4";
      if (a == 5) path = "level3/right";
      if (a == 6) path = "level3/right_2";
      if (a == 7) path = "level3/right_3";
      if (a == 8) path = "level3/right_4";
      Color color;
      a = UtilityFunctions::randi_range(1, 3);
      if (a == 1) color = blue; 
      if (a == 2) color = red; 
      if (a == 3) color = yellow;
      make_enemy(j, IMP,color, get_path2d(path),20);
    } 
  }// 10.5
  for (int i = 0; i < 7; i++) {
    double t1 = t + 14.5 + double(i) * 0.285;
    Color color;
    int a = UtilityFunctions::randi_range(1, 4);
    if (a == 1) color = blue; 
    if (a == 2) color = green; 
    if (a == 3) color = yellow;
    if (a == 4) color = brown;
    make_enemy(t1, Rotate, color, get_path2d("level3/up_4"),20);
  }// 16.5
  for (int i = 0; i < 7; i++) {
    double t1 = t + 17.5 + double(i) * 0.285;
    Color color;
    int a = UtilityFunctions::randi_range(1, 4);
    if (a == 1) color = blue; 
    if (a == 2) color = green; 
    if (a == 3) color = yellow;
    if (a == 4) color = brown;
    make_enemy(t1, Rotate, color, get_path2d("level3/up_2"),20);
  }// 19.5
  for (int i = 0; i < 7; i++) {
    double t1 = t + 20.5 + double(i) * 0.285;
    Color color;
    int a = UtilityFunctions::randi_range(1, 4);
    if (a == 1) color = blue; 
    if (a == 2) color = green; 
    if (a == 3) color = yellow;
    if (a == 4) color = brown;
    make_enemy(t1, Rotate, color, get_path2d("level3/up_1"),20);
  }// 22.5
  for (int i = 0; i < 7; i++) {
    double t1 = t + 23.5 + double(i) * 0.285;
    Color color;
    int a = UtilityFunctions::randi_range(1, 4);
    if (a == 1) color = blue; 
    if (a == 2) color = green; 
    if (a == 3) color = yellow;
    if (a == 4) color = brown;
    make_enemy(t1, Rotate, color, get_path2d("level3/up_3"),20);
  }// 25.5
  for (int i = 0; i < 7; i++) {
    double t1 = t + 26.5 + double(i) * 0.285;
    Color color;
    int a = UtilityFunctions::randi_range(1, 4);
    if (a == 1) color = blue; 
    if (a == 2) color = green; 
    if (a == 3) color = yellow;
    if (a == 4) color = brown;
    make_enemy(t1, Rotate, color, get_path2d("level3/up_1"),20);
  }// 28.5
  for (int i = 0; i < 7; i++) {
    double t1 = t + 29.5 + double(i) * 0.285;
    Color color;
    int a = UtilityFunctions::randi_range(1, 4);
    if (a == 1) color = blue; 
    if (a == 2) color = green; 
    if (a == 3) color = yellow;
    if (a == 4) color = brown;
    make_enemy(t1, Rotate, color, get_path2d("level3/up_2"),20);
  }// 31.5
  for (int i = 0; i < 7; i++) {
    double t1 = t + 32.5 + double(i) * 0.285;
    Color color;
    int a = UtilityFunctions::randi_range(1, 4);
    if (a == 1) color = blue; 
    if (a == 2) color = green; 
    if (a == 3) color = yellow;
    if (a == 4) color = brown;
    make_enemy(t1, Rotate, color, get_path2d("level3/up_4"),20);
  }// 34.5
  for (int i = 0; i < 7; i++) {
    double t1 = t + 35.5 + double(i) * 0.285;
    Color color;
    int a = UtilityFunctions::randi_range(1, 4);
    if (a == 1) color = blue; 
    if (a == 2) color = green; 
    if (a == 3) color = yellow;
    if (a == 4) color = brown;
    make_enemy(t1, Rotate, color, get_path2d("level3/up_1"),20);
  }// 37.5
  make_enemy(t + 39.5, BIG_butterfly,red, get_path2d("level3/up_down"),100);
  make_enemy(t + 39.5, BIG_butterfly,red, get_path2d("level3/up_down_2"),100);

  make_boos(t + 44.5, NITORI, 1);
}


void LevelManager::make_boos(double time, enemy_typ typ, int level){
  enemy boos ;
  boos.typ = typ;
  boos.level = level;
  level_timeline.emplace(time, std::move(boos));
}

void LevelManager::restart(){
  godot::SceneTree *tree = get_tree();
  if (tree) {
    // 调用引擎内置方法重新加载当前场景
    godot::Error err = tree->reload_current_scene();
    
    if (err != godot::OK) {
      godot::UtilityFunctions::printerr( err);
    }
  }
};

LevelManager::LevelManager() {
    // 当 Godot 实例化 Autoload 时，把实例赋给静态指针
  if (singleton == nullptr) {
      singleton = this;
  }
}

LevelManager::~LevelManager(){
if (singleton == this) {
  singleton = nullptr;
}
}

godot::Path2D *game::LevelManager::get_path2d(godot::String path){
  // 3. 根据相对路径寻找 Path2D
  // 假设在你的关卡场景里，Path2D 节点的名字叫 "EnemyPath"
  // 如果它在更深的层级，可以用 "Map/Paths/EnemyPath"
  godot::Node* target_node = current_scene->get_node_or_null("move/" + path);
  if (!target_node) {
    godot::UtilityFunctions::print("not found", path);
    return nullptr;
  }
  // 4. 安全类型转换
  godot::Path2D* path_obj = godot::Object::cast_to<godot::Path2D>(target_node);
  if (!path_obj){
    godot::UtilityFunctions::print("not found", path);
  } 
  return path_obj;
}

enemy::Minion* LevelManager::get_minion(enemy_typ typ, Color color){
  game::enemy::Minion* spawn_enemy = nullptr;
  // --- 1. 处理 IMP (妖精小怪) 类型 ---
  if (typ == IMP) { // 核心修复：= 改为 ==
    auto imp = memnew(game::enemy::Imp);
    
    // 根据传入的颜色，加载对应的 SpriteFrames 动画资源
    switch (color) {
      case blue:
        imp->set_animation("res://material/enemy/imp/hat/blue.tres");
        break;
      case red:
        imp->set_animation("res://material/enemy/imp/hat/red.tres");
        break;
      case yellow:
        imp->set_animation("res://material/enemy/imp/hat/yellow.tres");
        break;
      case pink_green:
        imp->set_animation("res://material/enemy/imp/pink_green.tres");
        break;
      case yellow_blue:
        imp->set_animation("res://material/enemy/imp/yellow_blue.tres");
        break;
      case yellow_red:
        imp->set_animation("res://material/enemy/imp/yellow_red.tres");
        break;
    }
    spawn_enemy = imp;
  }else if (typ == BIG_butterfly) {
    auto imp = memnew(game::enemy::Imp);
    switch (color) {
      case red:
        imp->set_animation("res://material/enemy/big_butterfly/red.tres");
        break;
    }
    spawn_enemy = imp;
  }else if (typ == Rotate) {
    auto rot = memnew(game::enemy::Rotate);
    switch (color) {
      case blue:
        rot->set_animation("res://material/enemy/rotate/blue.tres");
        break;
      case green:
        rot->set_animation("res://material/enemy/rotate/green.tres");
        break;
      case yellow:
        rot->set_animation("res://material/enemy/rotate/yellow.tres");
        break;
      case brown:
        rot->set_animation("res://material/enemy/rotate/brown.tres");
        break;
    }
    spawn_enemy = rot;
  }
  // --- 3. 安全防御与返回 ---
  if (spawn_enemy == nullptr) {
    godot::UtilityFunctions::print("LevelManager: Unknown enemy type requested!");
  }
  return spawn_enemy;
}

void game::LevelManager::make_enemy(
    double time,
    enemy_typ typ,
    godot::Path2D *path,
    std::vector<std::unique_ptr<utility::Move>> moves)
{
  enemy senemy ;
  senemy.moves = std::move(moves) ;
  senemy.path = path;
  senemy.typ = typ ;
  level_timeline.emplace(time, std::move(senemy));
}

void game::LevelManager::make_enemy(double time, enemy_typ typ, Color color, godot::Path2D *path, double hp){
  enemy senemy ;
  senemy.path = path;
  senemy.typ = typ ;
  senemy.coler = color;
  senemy.hp = hp;
  level_timeline.emplace(time, std::move(senemy));
}

LevelManager *game::LevelManager::get_singleton(){
  return singleton;
}
