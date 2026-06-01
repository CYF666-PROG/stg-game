#include "circle.hpp"

using namespace utility::move;

void utility::move::Circle::move(godot::Vector2 &this_position, double delta){
    // 1. 安全检查 
    if (time <= 0.0 || d == 0.0) return; 
    if (current_rad >= rad) return; 

    // 2. 计算本帧固定推进的弧度步长 
    const double BASE_FPS = 60.0; 
    double total_frames = time * BASE_FPS; 
    double step_rad = rad / total_frames; 

    // 限制最后一帧不要转过头 
    if (current_rad + step_rad > rad) { 
        step_rad = rad - current_rad; 
    } 
    current_rad += step_rad; 

    // 3. 决定旋转方向（正数顺时针，负数逆时针） 
    double delta_rad = (d > 0.0) ? step_rad : -step_rad; 

    // 4. 用最严谨的几何公式，直接推导“当前帧的位移向量” 
    // 弦长公式：弦长 = 2 * R * sin(theta / 2) 
    double chord_length = 2.0 * std::abs(d) * std::sin(step_rad / 2.0); 

    // 位移方向：当前朝向向量旋转 (delta_rad / 2) 的角度 
    godot::Vector2 move_direction = orientation.rotated(delta_rad / 2.0); 
    
    // 计算出本帧完整的二维位移向量
    godot::Vector2 full_displacement = move_direction * chord_length;

    // 5. 根据不同模式，对位移向量进行过滤应用
    if (typ == Normal){ 
        // 普通模式：全方向正常移动
        this_position += full_displacement; 
    }else if (typ == Horizontal){ 
        // 水平模式：只在 X 轴移动，Y 轴锁定（物体会像单摆一样左右往复或横向平移，但朝向照常旋转）
        this_position.x += full_displacement.x;
    }else if (typ == Vertical){ 
        // 竖直模式：只在 Y 轴移动，X 轴锁定
        this_position.y += full_displacement.y;
    } 

    // 6. 无论何种模式，朝向都必须严格同步旋转，确保下一帧的几何关系正确
    orientation = orientation.rotated(delta_rad); 
}


Circle::Circle() {}

utility::move::Circle::Circle(double spead, double d, double time, double x, double y, TYP typ) : time(time), d(d), typ(typ) {
  rad = spead*time;
  orientation.x = x; orientation.y = y ;
  // 设为单位向量
  orientation.normalize();
}

Circle::~Circle() {}