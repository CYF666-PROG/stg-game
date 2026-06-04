#include "../bullet_manager.hpp"
#include "../bullet_pool.hpp"
#include "../../input/keyboard.hpp"
#include "transmitter_1.hpp"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/sprite_frames.hpp>

namespace game::player {
class Transmitter_2 : public Transmitter_1{
  GDCLASS(Transmitter_2, Transmitter_1)
private:

public:

  void _ready() override;
  void frame_do(double delta) override;
  void shoot() override;
  static void _bind_methods();
  Transmitter_2();
  virtual ~Transmitter_2();
};
}