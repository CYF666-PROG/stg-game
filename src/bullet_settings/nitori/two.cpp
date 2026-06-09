#include "two.hpp"
#include "godot_cpp/classes/engine.hpp"

using namespace game::bullet_settings::nitori;

Two::Two(){
}

Two::~Two(){
}

void Two::_ready(){
    left.resize(8);
    right.resize(8);
    
    for (int i = 0; i < 8; i++) {
        left[i] = std::make_unique<Wave>();
        right[i] = std::make_unique<Wave>();
        
        left[i]->direction = 1;   // 向左
        right[i]->direction = -1; // 向右
    }
    re_set(); 
}

void Two::re_set() {
    global_frame_counter = 0;
    state = 0;
    
    float padding_y = 0; 
    float start_y = min_y + padding_y;
    float end_y = max_y - padding_y;
    
    // 🌟 基础间距（等分为 7 个总间隔）
    float step_y = (end_y - start_y) / 7.0f; 
    
    // 🌟 整个垂直轨道的总闭环跨度
    // 8个发射点占了 7*step_y 的高度，完整的闭环长度应该加上 1 个 step_y，即 8*step_y
    float total_height_range = (end_y - start_y) + step_y; 

    // 👉 每个周期整体向下移动半个间隔 (0.5 * step_y)
    float cycle_scroll_offset = current_cycle * (step_y * 0.5f);

    float padding_x = 0; 

    for (int i = 0; i < 8; i++) {
        // --- 【左边一排计算】 ---
        // 左边第 i 个的基础相对高度
        float left_base_y = i * step_y;
        // 加上周期下移偏移量
        float left_shifted_y = left_base_y + cycle_scroll_offset;
        // 使用 fmodf 确保超出 total_height_range 后自动洗回顶部 (0)
        float left_final_y = start_y + fmodf(left_shifted_y, total_height_range);

        left[i]->shoot_pos = godot::Vector2(max_x - padding_x, left_final_y);
        left[i]->is_enabled = false;
        
        // --- 【右边一排计算】 ---
        // 👉 核心：初始状态空间错开半个间隔，所以基础高度直接 + 0.5f * step_y
        float right_base_y = (i * step_y) + (step_y * 0.5f);
        // 加上周期下移偏移量
        float right_shifted_y = right_base_y + cycle_scroll_offset;
        // 同样进行传送带取模判断
        float right_final_y = start_y + fmodf(right_shifted_y, total_height_range);

        right[i]->shoot_pos = godot::Vector2(min_x + padding_x, right_final_y);
        right[i]->is_enabled = false;
    }
}

void Two::_physics_process(double delta){
    ///检查是否处于编辑器
    if (godot::Engine::get_singleton()->is_editor_hint()){
      return;
    }
    global_frame_counter++;

    switch (state) {
        // 🔄 状态 0：从上到下依次开启
        case 0: {
            int index_to_enable = global_frame_counter / activate_interval;
            
            if (index_to_enable < 8) {
                if (global_frame_counter % activate_interval == 0) {
                    left[index_to_enable]->is_enabled = true;
                    right[index_to_enable]->is_enabled = true;
                }
            } else {
                state = 1;
                global_frame_counter = 0; 
            }
            break;
        }

        // ⏳ 状态 1：全部保持开启，等待一段时间
        case 1: {
            if (global_frame_counter >= keep_alive_frames) {
                state = 2; 
                global_frame_counter = 0;
            }
            break;
        }

        // 🛑 状态 2：从上到下依次关闭
        case 2: {
            int index_to_disable = global_frame_counter / activate_interval;
            
            if (index_to_disable < 8) {
                if (global_frame_counter % activate_interval == 0) {
                    left[index_to_disable]->is_enabled = false;
                    right[index_to_disable]->is_enabled = false;
                }
            } else {
                state = 3; 
                global_frame_counter = 0;
            }
            break;
        }

        case 3: {
            // 👉 增加大周期计数器，使得下一次调用 re_set 时触发向下滚动
            current_cycle++; 
            re_set(); 
            break;
        }
    }

    // 🚀 驱动发射器
    for (int i = 0; i < 8; i++) {
        if (left[i]->is_enabled) {
            left[i]->shoot(); 
        }
        if (right[i]->is_enabled) {
            right[i]->shoot();
        }
    }
}