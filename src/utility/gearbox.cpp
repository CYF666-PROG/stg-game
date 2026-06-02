#include "gearbox.hpp"

#include <godot_cpp/classes/object.hpp>


using namespace game ;
using namespace utility;
using namespace godot;


void utility::Gearbox::_ready(){
  // 连接内置信号 "area_entered" 到本对象的指定函数
  connect("area_entered", godot::Callable(this, "_on_area_entered"));
}

void utility::Gearbox::change_spead(){
}

void utility::Gearbox::_on_area_entered(godot::Area2D *other_area){
  // 撞到怪
  enemy::Minion* minion = godot::Object::cast_to<enemy::Minion>(other_area);
  if (minion){
    hited_minion.insert({frame, minion});
  }
}

void utility::Gearbox::_bind_methods() {}

void utility::Gearbox::_physics_process(double delta){
int delay_frames = int(delay * 60.0);
  // 必须遍历整个容器，因为“正在变速”的小怪也需要每帧更新速度
  for (auto it = hited_minion.begin(); it != hited_minion.end(); ) {
    int hit_frame = it->first;
    enemy::Minion* minion = it->second;

    // 安全检查：防止小怪中途死亡导致野指针崩溃
    if (!godot::Object::is_valid_instance(minion)) {
      hited_minion.erase(it++);
      continue;
    }

    // 计算当前距离该小怪挨撞，过去了多少帧
    int passed_frames = frame - hit_frame;

    // 阶段 1：还在【等待期】
    if (passed_frames < delay_frames) {
        it++; // 啥也不做，老老实实等下一帧
        continue;
    }

    // ---------------- 进到这里，说明已经度过了等待期，开始平滑变速 ----------------
    
    double v0 = minion->speed; // 获取怪物的原生初始速度（避免数值污染）
    double vt = target_speed;

    // 依据物理公式推算：从 v0 变到 vt 走满 distance 到底需要多少帧
    double speed_sum = v0 + vt;
    if (speed_sum <= 0.1) speed_sum = 0.1; // 防止除以 0
    int total_change_frames = int((2.0 * distance / speed_sum) * 60.0);
    if (total_change_frames < 1) total_change_frames = 1; 

    // 计算当前处于【变速期】的第几帧
    int current_change_frame = passed_frames - delay_frames;

    // 阶段 2：处于【变速期】内
    if (current_change_frame < total_change_frames) {
        // 计算当前帧的平滑进度百分比 (0.0 ~ 1.0)
        double progress = (double)current_change_frame / (double)total_change_frames;
        
        // 线性插值（Lerp）算出当前帧的绝对速度
        double next_speed = v0 + (vt - v0) * progress;
        
        minion->speed = next_speed; // 平滑应用
        it++; // 移动到下一个小怪，这一只下一帧继续算
    } 
    // 阶段 3：变速完成，达到【最终期】
    else {
        minion->speed = vt; // 精准锁死目标速度
        hited_minion.erase(it++); // 功成身退，彻底从 multimap 移除！
    }
  }

  frame++; // 驱动全局物理帧递增

}

Gearbox::Gearbox() {}

Gearbox::~Gearbox(){}