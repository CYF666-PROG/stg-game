#include "gearbox.hpp"

#include <godot_cpp/classes/object.hpp>


using namespace game ;
using namespace utility;
using namespace godot;



bool utility::Gearbox::change_speed(int hit_frame, game::enemy::Minion* minion, double initial_speed){
  if (!minion) return true; // 小怪不存在，变速完成
  
  // 1. 计算延迟帧数（使用 std::round 避免浮点数精度截断）
  int delay_frames = static_cast<int>(std::round(delay * 60.0));
  int active_frames = frame - hit_frame - delay_frames;
  
  // 如果当前帧还没走到延迟结束，维持初始速度，返回 false 继续等待
  if (active_frames < 0) {
    minion->speed = initial_speed; 
    return false; 
  }
  
  // 2. 如果设定用时小于等于 0，或者初速度已经等于目标速度，直接完成变速
  if (time_taken <= 0.0 || std::abs(target_speed - initial_speed) < 0.001) {
    minion->speed = target_speed;
    return true;
  }
  
  // 3. 计算实际过去的时间（秒）以及进度比例
  double elapsed_time = active_frames / 60.0;
  double progress = elapsed_time / time_taken;
  
  // 4. 根据进度更新速度
  if (progress >= 1.0) {
    minion->speed = target_speed;
    return true; // 达到或超过目标时间，变速结束
  } else {
    // 匀变速直线运动：速度随时间线性插值
    minion->speed = initial_speed + (target_speed - initial_speed) * progress;
    return false; // 变速中
  }
}

void utility::Gearbox::_on_area_entered(godot::Area2D *other_area){
  // 撞到怪
  enemy::Minion* minion = godot::Object::cast_to<enemy::Minion>(other_area);
  if (minion){
    ///确保是自己轨迹上的怪
    if (get_parent()->get_parent() != minion->get_parent()->get_parent()) return;
    // 记录小怪的初始速度和id
    MinionHitInfo minion_hit_info;
    minion_hit_info.object_id = minion->get_instance_id();
    minion_hit_info.initial_speed = minion->speed;
    hited_minion.insert({frame, minion_hit_info});
  }
}


void utility::Gearbox::_physics_process(double delta){
  int delay_frames = int(delay * 60.0);
  // 必须遍历整个容器，因为“正在变速”的小怪也需要每帧更新速度
  for (auto it = hited_minion.begin(); it != hited_minion.end(); it++) {
    int hit_frame = it->first;
    // 使用ObjectID获取小怪实例，转换为Minion指针
    godot::Object* base_obj = ObjectDB::get_instance(ObjectID(it->second.object_id));
    enemy::Minion* minion = godot::Object::cast_to<enemy::Minion>(base_obj);
    // 安全检查：防止小怪中途死亡导致野指针崩溃
    if (!minion) {
      hited_minion.erase(it);
      continue;
    }
    // 计算当前距离该小怪挨撞，过去了多少帧
    int passed_frames = frame - hit_frame;
    // 阶段 1：还在【等待期】
    if (passed_frames < delay_frames) {
      // 啥也不做，老老实实等下一帧
      continue;
    }
    if(change_speed(hit_frame, minion, it->second.initial_speed)){
      hited_minion.erase(it);
    }
  }
  frame++; // 驱动全局物理帧递增
}

Gearbox::Gearbox() {}

Gearbox::~Gearbox(){}

void utility::Gearbox::_bind_methods() {
  // ---- 注册方法 (Methods) ----
  ClassDB::bind_method(D_METHOD("_on_area_entered"), &Gearbox::_on_area_entered);
  // 必须先注册 Getter/Setter 方法，Godot 才能通过它们来读写属性
  ClassDB::bind_method(D_METHOD("set_delay", "delay"), &Gearbox::set_delay);
  ClassDB::bind_method(D_METHOD("get_delay"), &Gearbox::get_delay);
  
  ClassDB::bind_method(D_METHOD("set_target_speed", "target_speed"), &Gearbox::set_target_speed);
  ClassDB::bind_method(D_METHOD("get_target_speed"), &Gearbox::get_target_speed);
  
  ClassDB::bind_method(D_METHOD("set_time_taken", "time_taken"), &Gearbox::set_time_taken);
  ClassDB::bind_method(D_METHOD("get_time_taken"), &Gearbox::get_time_taken);
  
  // ---- 注册属性 (Properties) ----
  // 参数说明：
  // 1. 变量类型 (PropertyInfo)
  // 2. 绑定的 Setter 方法名
  // 3. 绑定的 Getter 方法名
  
  // 注册 delay 属性
  ClassDB::add_property("Gearbox", PropertyInfo(Variant::FLOAT, "delay"), "set_delay", "get_delay");
  // 注册 target_speed 属性
  ClassDB::add_property("Gearbox", PropertyInfo(Variant::FLOAT, "target_speed"), "set_target_speed", "get_target_speed");
  // 注册 distance 属性
  ClassDB::add_property("Gearbox", PropertyInfo(Variant::FLOAT, "time_taken"), "set_time_taken", "get_time_taken");
}
  
void Gearbox::set_delay(const double p_delay) { delay = p_delay; }
double Gearbox::get_delay() const { return delay; }

void Gearbox::set_target_speed(const double p_speed) { target_speed = p_speed; }
double Gearbox::get_target_speed() const { return target_speed; }

void Gearbox::set_time_taken(const double p_distance) { time_taken = p_distance; }
double Gearbox::get_time_taken() const { return time_taken; }

void utility::Gearbox::_ready(){
  // 清除所有碰撞层
  set_collision_layer(0);
  set_collision_mask(0);
  // 设置自身为第5层
  set_collision_layer_value(5, true);
  // 检测第3层
  set_collision_mask_value(3, true);
  // 连接内置信号 "area_entered" 到本对象的指定函数
  connect("area_entered", godot::Callable(this, "_on_area_entered"));
}