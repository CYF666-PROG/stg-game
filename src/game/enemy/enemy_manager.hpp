#pragma once

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <vector>

namespace game::enemy {

class EnemyManager : public godot::Node {
    GDCLASS(EnemyManager, godot::Node);

// 定义一个结构体，把指针和 ID 绑定在一起
struct EnemyRef {
    godot::Node2D* pointer;
    uint64_t id;
};

private:
    static EnemyManager *instance;
    std::vector<EnemyRef> active_enemies; // 存储结构体列表

protected:
    static void _bind_methods();

public:
    EnemyManager();
    ~EnemyManager();

    static EnemyManager *get_singleton();
    /// @brief 注册敌人
    /// @param enemy 
    void register_enemy(godot::Node2D *enemy);
    /// @brief 注销敌人
    /// @param enemy 
    void unregister_enemy(godot::Node2D *enemy);
    /// @brief 查找敌人
    /// @param bullet_position 位置
    /// @param max_distance 离这个位置最大距离
    /// @return 
    godot::Node2D *get_closest_enemy(godot::Vector2 bullet_position, double max_distance);
    void clear_enemies();
    virtual void _ready() override ;
};
};
 // namespace godot