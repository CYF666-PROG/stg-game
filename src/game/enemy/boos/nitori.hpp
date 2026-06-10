#include "../boos.hpp"

namespace game::boos{
class NiToRi : public game::Boos {
  GDCLASS(NiToRi, game::Boos);

private:
protected:
public:
  static void _bind_methods(){};
  void _ready() override;

  void start_0() override;

  NiToRi() = default;
  ~NiToRi() = default;
};
}