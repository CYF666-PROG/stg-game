#include "circle.hpp"

using namespace utility::move;

void utility::move::Circle::move(godot::Vector2 &this_position, double delta){
  if (time <= 0.0 || d == 0.0) return; 
  if (current_rad >= rad) return; 

  const double BASE_FPS = 60.0; 
  double total_frames = time * BASE_FPS; 
  double step_rad = rad / total_frames; 

  if (current_rad + step_rad > rad) { 
      step_rad = rad - current_rad; 
  } 
  current_rad += step_rad; 

  double delta_rad = (d > 0.0) ? step_rad : -step_rad; 

  double chord_length = 2.0 * std::abs(d) * std::sin(step_rad / 2.0); 

  godot::Vector2 move_direction = orientation.rotated(delta_rad / 2.0); 
  
  godot::Vector2 full_displacement = move_direction * chord_length;

  if (typ == Normal){ 
      this_position += full_displacement; 
  }else if (typ == Horizontal){ 
      this_position.x += full_displacement.x;
  }else if (typ == Vertical){ 
      this_position.y += full_displacement.y;
  } 

  orientation = orientation.rotated(delta_rad); 
}


Circle::Circle() {}

utility::move::Circle::Circle(double spead, double d, double time, double x, double y, TYP typ) : time(time), d(d), typ(typ) {
  rad = spead*time;
  orientation.x = x; orientation.y = y ;
  orientation.normalize();
}

Circle::~Circle() {}