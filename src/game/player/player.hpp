#pragma once

#include "../entity.hpp"
#include "../../bullet_settings/flower_bullet.hpp"


namespace game{

class Player : public Entity{
  GDCLASS(Player, game::Entity)
private:
  godot::Ref<godot::Texture2D> bullet_texture ;
  bullet_settings::FlowerBullet* flow = nullptr;
  int orb_count = 4;
  bool is_slow = false;
  bool is_skilling = false;
public:
  void update();
  void move();
  void shoot();
  void check_orb();
  void skill();

  void update_animation() override;
  void entity_physics_process(double delta) override ;
  static void _bind_methods();
  void set_bullet_texture(const godot::Ref<godot::Texture2D> p_tex) { bullet_texture = p_tex; }
  godot::Ref<godot::Texture2D> get_bullet_texture() const { return bullet_texture; }
  void _ready() override ;

  Player();
  virtual ~Player();
};
}