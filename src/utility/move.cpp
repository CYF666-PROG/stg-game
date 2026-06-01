#include "move.hpp"

using namespace utility;

void Move::move(godot::Vector2& this_position, double delta){
  this_position += spead ;
  return ;
}

Move::Move(){}

utility::Move::Move(double spead, double x, double y){
  this->spead.x = x; this->spead.y = y;
  this->spead.normalize();
  this->spead *= (spead/60) ;
}

Move::Move(godot::Vector2 spead) : spead(spead){}

Move::~Move() {}