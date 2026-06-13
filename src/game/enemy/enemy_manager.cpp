#include "enemy_manager.hpp"

#include <godot_cpp/core/class_db.hpp>

#include <algorithm>

using namespace godot;
using namespace game::enemy;

EnemyManager *EnemyManager::instance = nullptr;

void EnemyManager::_bind_methods() {}

EnemyManager::EnemyManager() {}

void EnemyManager::_ready(){
  instance = this;
};

EnemyManager::~EnemyManager() { if (instance == this) instance = nullptr; }
EnemyManager *EnemyManager::get_singleton() { return instance; }

void EnemyManager::register_enemy(Node2D *enemy) {
  if (!enemy) return;
  uint64_t id = enemy->get_instance_id();
  // 检查是否已经注册过
  auto it = std::find_if(active_enemies.begin(), active_enemies.end(), [id](const EnemyRef& ref) {
    return ref.id == id;
  });
  if (it == active_enemies.end()) {
    active_enemies.push_back({enemy, id});
  }
}

void EnemyManager::unregister_enemy(Node2D *enemy) {
  if (!enemy) return;
  
  uint64_t id = enemy->get_instance_id();
  // 使用id查找结构体数组
  auto it = std::find_if(active_enemies.begin(), active_enemies.end(), [id](const EnemyRef& ref) {
    return ref.id == id;
  });

  if (it != active_enemies.end()) {
    active_enemies.erase(it);
  }
}

Node2D *EnemyManager::get_closest_enemy(Vector2 bullet_position, double max_distance) {
  Node2D *closest_enemy = nullptr;
  double min_distance_sq = max_distance * max_distance; 

  // 遍历敌人
  for (auto it = active_enemies.begin(); it != active_enemies.end(); ) {
      
    // 使用ID检测对象是否存活
    Object* obj = ObjectDB::get_instance(it->id);
    // 强转 Node2D，检查是否在场景树
    auto* enemy = Object::cast_to<Node2D>(obj);
    if (enemy == nullptr || !enemy->is_inside_tree()) {
      // 发现野指针从列表中剔除，并且迭代器不往后移!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      it = active_enemies.erase(it);
      continue; 
    }
    // 指针雀实有效
    double dist_sq = bullet_position.distance_squared_to(enemy->get_global_position());
    if (dist_sq < min_distance_sq) {
      min_distance_sq = dist_sq;
      closest_enemy = enemy;
    }
    ++it; // 迭代器自增 移动到下一个敌人
  }
  return closest_enemy;
}

void EnemyManager::clear_enemies() {
  active_enemies.clear();
}