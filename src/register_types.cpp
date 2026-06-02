#include "register_types.hpp"
#include "game/entity.hpp"
#include "game/player.hpp"
#include "input/keyboard.hpp"
#include "game/levelmanager.hpp"
#include "game/enemy/enemy.hpp"
#include "game/enemy/minion.hpp"
#include "game/enemy/imp.hpp"
#include "game/bullet_manager.hpp"
#include "bullet_settings/flower_bullet.hpp"
#include "game/bullet/player_bullet.hpp"
#include "game/bullet/round_bullet.hpp"
#include "utility/trigger.hpp"
#include "utility/gearbox.hpp"
#include "game/bullet_pool.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>

using namespace godot ;

void initialize_example_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
  GDREGISTER_CLASS(game::Entity);
  GDREGISTER_CLASS(game::Player);
  GDREGISTER_CLASS(input::KeyBoard);
  GDREGISTER_CLASS(game::Bullet);
  GDREGISTER_CLASS(game::LevelManager);
  GDREGISTER_CLASS(game::Enemy)
  GDREGISTER_CLASS(game::enemy::Minion)
  GDREGISTER_CLASS(game::enemy::Imp)
  GDREGISTER_CLASS(game::BulletManager);
  GDREGISTER_CLASS(game::bullet_settings::FlowerBullet);
  GDREGISTER_CLASS(game::bullet::PlayerBullet);
  GDREGISTER_CLASS(game::bullet::RoundBullet);
  GDREGISTER_CLASS(utility::Trigger);
  GDREGISTER_CLASS(utility::Gearbox);
  GDREGISTER_CLASS(game::BulletPool);

  memnew(input::KeyBoard);
  Engine::get_singleton()->register_singleton(
    "KeyBoard", 
    input::KeyBoard::get_singleton()
  );
}

void uninitialize_example_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

  Engine::get_singleton()->unregister_singleton("KeyBoard");
  memdelete(input::KeyBoard::get_singleton());
}

extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT example_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

	init_obj.register_initializer(initialize_example_module);
	init_obj.register_terminator(uninitialize_example_module);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}
}