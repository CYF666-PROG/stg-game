#include "gearbox.hpp"
#include <godot_cpp/classes/object.hpp>
#include <cmath>

using namespace game;
using namespace utility;
using namespace godot;

bool utility::Gearbox::change_speed(
  int hit_frame, 
  game::enemy::Minion* minion, 
  double initial_speed, 
  double &acc_dist
) {
  if (!minion) return true; // 小怪不存在，变速完成

  const double DELTA = 1.0 / 60.0; // 固定物理帧间隔时间

  // 计算延迟帧数
  int delay_frames = static_cast<int>(std::round(delay * 60.0));
  int active_frames = frame - hit_frame - delay_frames;

  // 如果当前帧还没走到延迟结束 维持初始速度 重置累加距离 继续等待
  if (active_frames < 0) {
    minion->speed = initial_speed; 
    acc_dist = 0.0; 
    return false; 
  }

  // 如果设定的目标距离小于等于 0 或者速度本来就一样 直接同步目标速度
  if (target_distance <= 0.0 || std::abs(target_speed - initial_speed) < 0.001) {
    minion->speed = target_speed;
    return true;
  }

  // 利用小怪当前的速度计算这一物理帧走过的像素位移 将其累加进该小怪的专属引用中
  double current_frame_displacement = minion->speed * DELTA;
  acc_dist += current_frame_displacement;

  // 计算距离进度比例
  double progress = acc_dist / target_distance;

  if (progress >= 1.0) {
    minion->speed = target_speed;
    return true; // 走满了指定的变速距离，变速结束
  } else {
    // 速度关于 距离进度 进行线性插值
    minion->speed = initial_speed + (target_speed - initial_speed) * progress;
    return false; // 仍在变速距离内，继续驱动
  }
}

void utility::Gearbox::_on_area_entered(godot::Area2D *other_area) {
  auto* minion = godot::Object::cast_to<enemy::Minion>(other_area);
  if (minion) {
    // 确保是自己轨迹树上的怪
    if (get_parent()->get_parent() != minion->get_parent()->get_parent()) return;
    
    MinionHitInfo minion_hit_info;
    minion_hit_info.object_id = minion->get_instance_id();
    minion_hit_info.initial_speed = minion->speed;
    minion_hit_info.accumulated_distance = 0.0; // 撞击瞬间初始为 0 像素
    hited_minion.insert({frame, minion_hit_info});
  }
}

void utility::Gearbox::_physics_process(double delta) {
  int delay_frames = int(delay * 60.0);
  
  for (auto it = hited_minion.begin(); it != hited_minion.end();) {
    int hit_frame = it->first;
    
    godot::Object* base_obj = ObjectDB::get_instance(ObjectID(it->second.object_id));
    auto* minion = godot::Object::cast_to<enemy::Minion>(base_obj);
    
    if (!minion) {
      it = hited_minion.erase(it); // 删除，并返回下一任迭代器
      continue;
    }
    
    int passed_frames = frame - hit_frame;
    if (passed_frames < delay_frames) {
      it++; // 还在延迟等待期，跳过并检查下一个
      continue;
    }
    
    if (change_speed(hit_frame, minion, it->second.initial_speed, it->second.accumulated_distance)) {
      it = hited_minion.erase(it); // 变速完成，移出容器
    } else {
      it++; // 还在变速中，迭代器自增
    }
  }
  frame++; 
}

Gearbox::Gearbox() {}
Gearbox::~Gearbox() {}

void utility::Gearbox::_bind_methods() {
  ClassDB::bind_method(D_METHOD("_on_area_entered"), &Gearbox::_on_area_entered);
  
  ClassDB::bind_method(D_METHOD("set_delay", "delay"), &Gearbox::set_delay);
  ClassDB::bind_method(D_METHOD("get_delay"), &Gearbox::get_delay);
  
  ClassDB::bind_method(D_METHOD("set_target_speed", "target_speed"), &Gearbox::set_target_speed);
  ClassDB::bind_method(D_METHOD("get_target_speed"), &Gearbox::get_target_speed);
  
  ClassDB::bind_method(D_METHOD("set_target_distance", "target_distance"), &Gearbox::set_target_distance);
  ClassDB::bind_method(D_METHOD("get_target_distance"), &Gearbox::get_target_distance);
  
  ClassDB::add_property("Gearbox", PropertyInfo(Variant::FLOAT, "delay"), "set_delay", "get_delay");
  ClassDB::add_property("Gearbox", PropertyInfo(Variant::FLOAT, "target_speed"), "set_target_speed", "get_target_speed");
  ClassDB::add_property("Gearbox", PropertyInfo(Variant::FLOAT, "target_distance"), "set_target_distance", "get_target_distance");
}
  
void Gearbox::set_delay(const double p_delay) { delay = p_delay; }
double Gearbox::get_delay() const { return delay; }

void Gearbox::set_target_speed(const double p_speed) { target_speed = p_speed; }
double Gearbox::get_target_speed() const { return target_speed; }

void Gearbox::set_target_distance(const double p_distance) { target_distance = p_distance; }
double Gearbox::get_target_distance() const { return target_distance; }

void utility::Gearbox::_ready() {
  set_collision_layer(0);
  set_collision_mask(0);
  set_collision_layer_value(5, true);
  set_collision_mask_value(3, true);
  connect("area_entered", godot::Callable(this, "_on_area_entered"));
}