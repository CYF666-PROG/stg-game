#include "enemy_manager.hpp"
#include <godot_cpp/core/class_db.hpp>

#include <algorithm>

using namespace godot;
using namespace game::enemy;

EnemyManager *EnemyManager::instance = nullptr;

void EnemyManager::_bind_methods() {
}

EnemyManager::EnemyManager() {}
void EnemyManager::_ready(){
  instance = this;
};
EnemyManager::~EnemyManager() { if (instance == this) instance = nullptr; }
EnemyManager *EnemyManager::get_singleton() { return instance; }

// 1. 注册时同时记录指针和 ID
void EnemyManager::register_enemy(Node2D *enemy) {
    if (!enemy) return;
    
    uint64_t id = enemy->get_instance_id();
    
    // 检查是否已经注册过（通过 ID 判断更安全）
    auto it = std::find_if(active_enemies.begin(), active_enemies.end(), [id](const EnemyRef& ref) {
        return ref.id == id;
    });

    if (it == active_enemies.end()) {
        active_enemies.push_back({enemy, id});
    }
}

// 2. 注销时通过 ID 查找并移除
void EnemyManager::unregister_enemy(Node2D *enemy) {
    if (!enemy) return;
    
    uint64_t id = enemy->get_instance_id();
    
    auto it = std::find_if(active_enemies.begin(), active_enemies.end(), [id](const EnemyRef& ref) {
        return ref.id == id;
    });

    if (it != active_enemies.end()) {
        active_enemies.erase(it);
    }
}

// 3. 寻敌时进行【双重保险】校验
Node2D *EnemyManager::get_closest_enemy(Vector2 bullet_position, double max_distance) {
    Node2D *closest_enemy = nullptr;
    double min_distance_sq = max_distance * max_distance; 

    // 因为遍历过程中可能会顺便清理掉失效的敌人，使用迭代器循环
    for (auto it = active_enemies.begin(); it != active_enemies.end(); ) {
        
        // 【第一道防线】：拿着 ID 去 Godot 数据库查证对象是否还活着
        Object* obj = ObjectDB::get_instance(it->id);
        
        // 【第二道防线】：将查出来的对象强转回 Node2D，并检查它是否在场景树中
        Node2D* enemy = Object::cast_to<Node2D>(obj);

        if (enemy == nullptr || !enemy->is_inside_tree()) {
            // 发现僵尸指针！说明这个敌人已经死了却没能成功取消注册。
            // 安全地从列表中将其剔除，并且迭代器不往后移
            it = active_enemies.erase(it);
            continue; 
        }

        // 走到这里，说明指针 100% 安全有效
        double dist_sq = bullet_position.distance_squared_to(enemy->get_global_position());
        if (dist_sq < min_distance_sq) {
            min_distance_sq = dist_sq;
            closest_enemy = enemy;
        }
        
        ++it; // 移动到下一个敌人
    }

    return closest_enemy;
}

void EnemyManager::clear_enemies() {
    active_enemies.clear();
}