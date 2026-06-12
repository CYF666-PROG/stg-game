#include "minion.hpp"

namespace game::enemy {
  /// @brief 旋转
class Rotate : public Minion{
  GDCLASS(Rotate, Minion)
private:

public:
  void update_animation() override;
  static void _bind_methods(){}
  void _ready() override ;
  Rotate() = default;
  /// 动画路径;
  virtual ~Rotate() = default;
};
}