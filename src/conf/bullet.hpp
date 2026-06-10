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
namespace pointed {
  inline const std::string path_blue = "res://material/bullet/pointed/blue.tres";
  /// 判定半径
  constexpr double radius = 3;
  /// 旋转偏移
  constexpr double rotation_offset = 1.570781325;
  /// 贴图缩放
  constexpr double scale = 1.7;
}
namespace ring {
  /// 判定半径
  constexpr double radius = 6;
  /// 旋转偏移
  constexpr double rotation_offset = 0;
  /// 贴图缩放
  constexpr double scale = 1.5;
}
namespace circle{
  /// 判定半径
  constexpr double radius = 12;
  /// 旋转偏移
  constexpr double rotation_offset = 0;
  /// 贴图缩放
  constexpr double scale = 2.187;
};
namespace fire {
  /// 判定半径
  constexpr double radius = 10;
  /// 旋转偏移
  constexpr double rotation_offset = 1.570781325;
  /// 贴图缩放
  constexpr double scale = 2;
}
namespace ellipse {
  /// 判定半径
  constexpr double radius = 5;
  /// 旋转偏移
  constexpr double rotation_offset = 1.570781325;
  /// 贴图缩放
  constexpr double scale = 1.5;
}
namespace rice {
  /// 判定半径
  constexpr double radius = 3;
  /// 旋转偏移
  constexpr double rotation_offset = 1.570781325;
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
    constexpr double scale = 1.5;
}

namespace blue_yin_yang_orb{
    inline const std::string path = "res://material/player/p1/blue_yin_yang_orb.tres";
    /// 锚点
    constexpr double anchor_x = 0.5;
    constexpr double anchor_y = 0.5;
    /// 贴图缩放
    constexpr double scale = 1.5;
}

namespace pink_yin_yang_orb{
    inline const std::string path = "res://material/player/p1/pink_yin_yang_orb.tres";
    /// 锚点
    constexpr double anchor_x = 0.5;
    constexpr double anchor_y = 0.5;
    /// 贴图缩放
    constexpr double scale = 1.5;
}

namespace skill_1{
    inline const std::string path = "res://material/player/p1/skill.tres";
    constexpr double scale = 5.69;
    /// 判定半径
    constexpr double radius = 132;
}
namespace heart {
  constexpr double scale = 1;
  inline const std::string path_0 = "res://material/ui/heart/0.tres";
  inline const std::string path_1 = "res://material/ui/heart/1.tres";
  inline const std::string path_2 = "res://material/ui/heart/2.tres";
  inline const std::string path_3 = "res://material/ui/heart/3.tres";
  inline const std::string path_4 = "res://material/ui/heart/4.tres";
  inline const std::string path_full = "res://material/ui/heart/full.tres";
}
namespace star {
  constexpr double scale = 1;
  inline const std::string path_0 = "res://material/ui/star/0.tres";
  inline const std::string path_1 = "res://material/ui/star/1.tres";
  inline const std::string path_2 = "res://material/ui/star/2.tres";
  inline const std::string path_3 = "res://material/ui/star/3.tres";
  inline const std::string path_4 = "res://material/ui/star/4.tres";
  inline const std::string path_full = "res://material/ui/star/full.tres";
}
namespace point {
  constexpr double scale = 2.041845;
  inline const std::string path = "res://material/player/p1/s_point.tres";
}
} // namespace player
namespace boos {
namespace nitori {
  inline const std::string path = "res://material/enemy/boos/nitori.tres";
  /// 锚点
  constexpr double anchor_x = 0.5;
  constexpr double anchor_y = 0.5;
  /// 贴图缩放
  constexpr double scale = 1.65;
}
namespace health {
  inline const std::string path_outside = "res://material/ui/boos_health/out_health.tres";
  inline const std::string path_inside = "res://material/ui/boos_health/in_health.tres";
  /// 锚点
  constexpr double anchor_x = 0.5;
  constexpr double anchor_y = 0.5;
  /// 贴图缩放
  constexpr double scale = 1;
}
}
} // namespace conf
