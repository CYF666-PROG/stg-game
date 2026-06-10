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
  void start_1() override;
  void start_2() override;
  void start_3() override;
  void start_4() override;

  NiToRi() = default;
  ~NiToRi() = default;
};
}