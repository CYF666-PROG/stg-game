shader_type canvas_item;

void fragment() {
    vec4 tex_color = texture(TEXTURE, UV);
    
    // 🌟 如果红绿蓝三个通道的值都很低（接近纯黑），就把透明度设为 0
    if (tex_color.r < 0.06 && tex_color.g < 0.06 && tex_color.b < 0.06) {
        discard; // 丢弃这一块的像素，直接变完全透明
    }
    
    COLOR = tex_color;
}