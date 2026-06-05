#pragma once
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <string>

namespace conf{
namespace bullet{
namespace player_1{
    inline const std::string path = "res://material/player/p1/fast_bullet_1.tres";
    /// 锚点
    constexpr double anchor_x = 0.8392857142857143;
    constexpr double anchor_y = 0.5;
    /// 判定半径
    constexpr double radius = 25;
    /// 旋转偏移
    constexpr double rotation_offset = -1.570781325;
    /// 贴图缩放
    constexpr double scale = 2;
}

namespace player_2{
    inline const std::string path = "res://material/player/p1/fast_bullet_2.tres";
    /// 锚点
    constexpr double anchor_x = 0.8392857142857143;
    constexpr double anchor_y = 0.5;
    /// 判定半径
    constexpr double radius = 25;
    /// 旋转偏移
    constexpr double rotation_offset = -1.570781325;
    /// 贴图缩放
    constexpr double scale = 2;
}

namespace player_3{
    inline const std::string path = "res://material/player/p1/fast_bullet_3.tres";
    /// 锚点
    constexpr double anchor_x = 0.8392857142857143;
    constexpr double anchor_y = 0.5;
    /// 判定半径
    constexpr double radius = 25;
    /// 旋转偏移
    constexpr double rotation_offset = -1.570781325;
    /// 贴图缩放
    constexpr double scale = 2;
}
} // namespace bullet

namespace player{
namespace red_yin_yang_orb{
    inline const std::string path = "res://material/player/p1/red_yin_yang_orb.tres";
    /// 锚点
    constexpr double anchor_x = 0.5;
    constexpr double anchor_y = 0.5;
    /// 贴图缩放
    constexpr double scale = 2;
}

namespace blue_yin_yang_orb{
    inline const std::string path = "res://material/player/p1/blue_yin_yang_orb.tres";
    /// 锚点
    constexpr double anchor_x = 0.5;
    constexpr double anchor_y = 0.5;
    /// 贴图缩放
    constexpr double scale = 2;
}

namespace pink_yin_yang_orb{
    inline const std::string path = "res://material/player/p1/pink_yin_yang_orb.tres";
    /// 锚点
    constexpr double anchor_x = 0.5;
    constexpr double anchor_y = 0.5;
    /// 贴图缩放
    constexpr double scale = 2;
}

namespace skill_1{
    inline const std::string path = "res://material/player/p1/skill.tres";
    constexpr double scale = 5.69;
    /// 判定半径
    constexpr double radius = 132;
}
} // namespace player
} // namespace conf
