#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/core/class_db.hpp>


namespace game::player{
class Skill : public godot::Area2D {
  GDCLASS(Skill, godot::Area2D)
private:
  // 是否开始爆炸
  bool is_boon = false;
  int is_to_boon = 0;
  godot::Sprite2D* spr = nullptr;
  // 存活帧数
  int live_frame = 0;
  void check_collisions();
  godot::Vector2 vector_speed = godot::Vector2(0,-1);
  /// @brief 极坐标角度
  double angle = 0;
  /// @brief 极坐标长度
  double lenght = 0;
  /// @brief 极坐标长度速度
  double speed = 0 ;
  /// @brief 极坐标速度
  double angle_speed = 0;
public:
  virtual void _on_area_entered(godot::Area2D *other_area);
  static void _bind_methods();
  void _physics_process(double delta) override;
  void _ready() override;
  Skill(double angle);
  Skill();
  ~Skill();
};
  

  
}