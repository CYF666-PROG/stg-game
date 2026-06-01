#pragma once

#include "../utility/move.hpp"

#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/node2d.hpp>

#include <map>
#include <functional>
#include <vector>
#include <memory>

namespace game{

class LevelManager : public godot::Node2D{
  GDCLASS(LevelManager, godot::Node2D)
private:
  enum enemy_typ {
    IMP
  };

  struct enemy {
    enemy_typ typ = IMP;
    /// 初始位置
    godot::Vector2 start_position{10,10};
    /// 轨迹组
    std::vector<std::unique_ptr<utility::Move>> moves;
  };
  /// 敌人时间表
  std::multimap<double, enemy> level_timeline;
  // 关卡计时器 单位为帧
  int level_frame = 0;
  
public:
  /// @brief 
  /// @param time 出场时间
  /// @param typ 类型
  /// @param start_position 出场位置
  /// @param moves 轨迹数组
  void make_enemy(
    double time,
    enemy_typ typ, 
    godot::Vector2 start_position, 
    std::vector<std::unique_ptr<utility::Move>> moves
  );
  void _physics_process(double delta) override;
  void _ready() override ;
  static void _bind_methods();
  LevelManager();
  virtual ~LevelManager();
};

} // namespace game
