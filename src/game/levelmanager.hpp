#pragma once

#include "../utility/move.hpp"
#include "enemy/minion.hpp"

#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/path2d.hpp>

#include <map>
#include <vector>
#include <memory>

namespace game{

class LevelManager : public godot::Node2D{
  GDCLASS(LevelManager, godot::Node2D)
private:
  static LevelManager* singleton;
  enum enemy_typ {
    IMP,
    NITORI,
    BIG_butterfly,
    Rotate
  };
  enum Color {
    blue,
    red,
    yellow,
    brown,
    green,
    pink_green,
    yellow_blue,
    yellow_red,
  };

  struct enemy {
    int level = 0;
    double hp = 100;
    Color coler = blue;
    enemy_typ typ = IMP;
    // 死亡后的掉落
    double power_up = 0;
    double hp_up = 0;
    double star_up = 0;
    /// 轨迹对象
    godot::Path2D* path = nullptr;
    /// 轨迹组
    std::vector<std::unique_ptr<utility::Move>> moves;
  };
  /// @brief 当前场景根节点
  godot::Node* current_scene = nullptr ;
  /// 敌人时间表 键为出现帧
  std::multimap<int, enemy> level_timeline;
  // 关卡计时器 单位为帧
  int level_frame = 0;
  /// @brief 获取move下path2d节点
  /// @param path 相对于move节点的路径 例如 "level1/LiftUp"
  /// @return 
  godot::Path2D* get_path2d(godot::String path);
  bool is_end = false;
  bool is_game_over_fading = false; // 是否已经进入淡出状态
  float fade_alpha = 0.0f;           // 当前暗度 (0.0 ~ 1.0)
  /// @brief 游戏结束场景变暗
  /// @param delta 帧间隔时间（秒）
  /// @param target_scene 目标场景路径
  /// @param fade_speed 变暗速度 透明度/秒
  void process_game_over_fade(double delta, godot::String target_scene, float fade_speed = 0.1f);

  
public:
  // 用于渲染黑色遮罩
  void _draw() override;
  // 设置子节点可见
  void set_all_children_visible();
  game::enemy::Minion* get_minion(enemy_typ typ, Color color);
  /// @brief 
  /// @param time 出场时间
  /// @param typ 类型
  /// @param path 轨迹对象
  /// @param hp 生命值
  /// @param power_up 掉落p点
  /// @param hp_up 掉落生命
  /// @param star_up 掉落星
  void make_enemy(
    double time,
    enemy_typ typ,
    Color color,
    godot::Path2D* path,
    double hp,
    double power_up = 1,
    double hp_up = 0,
    double star_up = 0
  );
  void make_boos(double time, enemy_typ typ, int level);
  bool is_pause = false;
  void pause();
  void start();
  int now_level = 1;
  void level_1();
  // 5:30
  double level_2_time = 17;
  void level_2();
  // 6:45
  double level_3_time = 75;
  void level_3();

  static LevelManager* get_singleton();
  void restart();
  void _physics_process(double delta) override;
  void _ready() override ;
  static void _bind_methods();
  LevelManager();
  virtual ~LevelManager();
};


} // namespace game
