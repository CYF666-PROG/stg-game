#include "levelmanager.hpp"
#include "../utility/move.hpp"
#include "../utility/move/circle.hpp"
#include "enemy/boos.hpp"
#include "enemy/boos/nitori.hpp"
#include "enemy/imp.hpp"
#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/core/memory.hpp"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/path_follow2d.hpp>


using namespace game;
using namespace godot;

LevelManager* LevelManager::singleton = nullptr;

void game::LevelManager::_physics_process(double delta)
{
  /// 检查是否在编辑器
  if (godot::Engine::get_singleton()->is_editor_hint()){
    return;
  }
  
  auto it_end = level_timeline.lower_bound(double(level_frame)/60);
  for (auto it = level_timeline.begin(); it != it_end;) {
    if (it->second.typ == IMP){
      auto* pf = memnew(godot::PathFollow2D);
      // 先设置禁用旋转，再添加
      pf->set_rotates(false);
      it->second.path->add_child(pf);
      game::enemy::Imp* imp = memnew(game::enemy::Imp("imp/1_blue.tres"));
      imp->path_follow = pf;
      pf->add_child(imp);
    }else if (it->second.typ == NITORI) {
      auto *current_scene_root = get_tree()->get_current_scene();
      if (!current_scene_root) {
        UtilityFunctions::print("LevelManager::_physics_process scene_root not foud");
        continue;
      }
      auto nitori = memnew(game::boos::NiToRi);
      current_scene_root->add_child(nitori);
      UtilityFunctions::print("make nitori");
    }
    it = level_timeline.erase(it);
  }
  level_frame += 1;
}

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

  make_enemy(0.5, NITORI, get_path2d("level1/LiftUp"));
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
    godot::UtilityFunctions::print("错误：在当前场景下未找到名为 LiftUp 的节点");
    return nullptr;
  }
  // 4. 安全类型转换
  godot::Path2D* path_obj = godot::Object::cast_to<godot::Path2D>(target_node);
  if (!path_obj){
    godot::UtilityFunctions::print("错误：在当前场景下未找到名为 LiftUp 的 Path2D 节点");
  } 
  return path_obj;
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

void game::LevelManager::make_enemy(double time, enemy_typ typ, godot::Path2D *path){
  enemy senemy ;
  senemy.path = path;
  senemy.typ = typ ;
  level_timeline.emplace(time, std::move(senemy));
}

LevelManager *game::LevelManager::get_singleton(){
  return singleton;
}
