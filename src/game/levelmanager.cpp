#include "levelmanager.hpp"
#include "../utility/move.hpp"
#include "../utility/move/circle.hpp"
#include "enemy/imp.hpp"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/engine.hpp>


using namespace game;


  
void game::LevelManager::_physics_process(double delta){
  /// 检查是否在编辑器
  if (godot::Engine::get_singleton()->is_editor_hint()){
    return;
  }

  auto it_end = level_timeline.lower_bound(double(level_frame)/60);
  for (auto it = level_timeline.begin(); it != it_end;) {
    if (it->second.typ == IMP){
      game::enemy::Imp* add = memnew(game::enemy::Imp);
      add->moves = std::move(it->second.moves);
      add->start_position = it->second.start_position;
      add_child(add);
      it = level_timeline.erase(it); 
    }
  }
  level_frame += 1;
}

void game::LevelManager::_bind_methods(){}

void game::LevelManager::_ready(){
  enemy add;
  std::vector<std::unique_ptr<utility::Move>> moves ;
  std::unique_ptr<utility::Move> yuan = std::make_unique<utility::move::Circle>(3,20,10,-1,0,utility::move::Circle::TYP::Vertical);
  yuan->start_time = 0; yuan->end_time = 5 ;
  moves.push_back(std::move(yuan));
  std::unique_ptr<utility::Move> zhi_xian = std::make_unique<utility::Move>(200,-1,0);
  zhi_xian->start_time = 0; zhi_xian->end_time = 5;
  moves.push_back(std::move(zhi_xian));
  make_enemy(0.5, IMP, godot::Vector2(1000,500), std::move(moves));
}

LevelManager::LevelManager() {}

LevelManager::~LevelManager(){}

void game::LevelManager::make_enemy(
  double time,
  enemy_typ typ, 
  godot::Vector2 start_position, 
  std::vector<std::unique_ptr<utility::Move>> moves
) {
  enemy senemy ;
  senemy.moves = std::move(moves) ;
  senemy.start_position = start_position;
  senemy.typ = typ ;
  level_timeline.emplace(time, std::move(senemy));
}