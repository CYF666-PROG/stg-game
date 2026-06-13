#include "card_name.hpp"
#include "godot_cpp/classes/label.hpp"
#include "godot_cpp/core/memory.hpp"

#include <godot_cpp/classes/font.hpp> 
#include <godot_cpp/classes/tween.hpp>
#include <godot_cpp/classes/property_tweener.hpp>
#include <godot_cpp/classes/global_constants.hpp>

using namespace ui;
using namespace godot;

void CardName::_bind_methods() {
    ClassDB::bind_method(D_METHOD("setup_and_play", "text"), &CardName::setup_and_play);
}

CardName::CardName() {}
CardName::~CardName() {}

void CardName::_ready() {
  label_node = memnew(Label);
  add_child(label_node);
  set_z_index(100);
}

void CardName::setup_and_play(const String &p_text) {
    if (!label_node) return;

    label_node->set_text(p_text);
    // 设置字体大小
    label_node->add_theme_font_size_override("font_size", 32);
    // 1. 精确测量中文字符串的绝对像素宽高
    Ref<Font> font = label_node->get_theme_font("font");
    int font_size = label_node->get_theme_font_size("font_size");
    
    Vector2 text_real_size = Vector2(0, 0);
    if (font.is_valid()) {
        text_real_size = font->get_string_size(p_text, HORIZONTAL_ALIGNMENT_LEFT, -1, font_size);
    }

    // 加上安全的内边距 Padding
    float final_width = text_real_size.x + 20.0f;
    float final_height = text_real_size.y + 10.0f;

    // 2. 强行重写当前节点和 Label 的尺寸，确保它们在当前帧就是这个大小
    Vector2 strict_size = Vector2(final_width, final_height);
    label_node->set_custom_minimum_size(strict_size);
    label_node->set_size(strict_size);
    set_custom_minimum_size(strict_size);
    set_size(strict_size); // 这一步至关重要，强制重写自身 Size

    // 3. 【重新修正坐标】
    // 因为 final_width 已经是准确的文字总长了，
    // 用 1920 减去 它的总长 再减去 边距，算出来的左上角 X 点会完美向左退回足够的空间。
    float exact_x = SCREEN_RIGHT_BOTTOM.x - final_width - margin;
    
    // 安全防御：如果字长到连屏幕都塞不下了，强制让 X 等于边距（靠最左），防止左边也震荡超出
    if (exact_x < margin) {
        exact_x = margin;
    }

    // 起始点（右下角）
    Vector2 real_start_pos = Vector2(
        exact_x,
        SCREEN_RIGHT_BOTTOM.y - final_height - margin
    );

    // 结束点（右上角）
    Vector2 real_end_pos = Vector2(
        exact_x,
        SCREEN_LEFT_TOP.y + margin
    );

    // 4. 设置位置并播放动画
    set_position(real_start_pos);
    set_modulate(Color(1.0f, 1.0f, 1.0f, 0.0f));

    Ref<Tween> tween = create_tween();
    if (tween.is_valid()) {
        tween->set_parallel(true);
        tween->tween_property(this, "position", real_end_pos, duration)
             ->set_trans(Tween::TRANS_CUBIC)
             ->set_ease(Tween::EASE_OUT);
             
        tween->tween_property(this, "modulate:a", 1.0f, duration * 0.3);
    }
}