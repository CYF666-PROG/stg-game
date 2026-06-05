#include "register_types.hpp"
#include "game/entity.hpp"
#include "game/player/player.hpp"
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
#include "game/player/transmitter_1.hpp"
#include "game/effect_manager.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>

using namespace godot ;

// 1. 在文件顶部（using namespace godot; 下面），定义两个专属于这个文件的静态追踪指针
static input::KeyBoard* _keyboard_instance = nullptr;
static game::LevelManager* _levelmanager_instance = nullptr;

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
  GDREGISTER_CLASS(game::player::Transmitter_1);
  GDREGISTER_CLASS(game::EffectManager);

}

void uninitialize_example_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

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