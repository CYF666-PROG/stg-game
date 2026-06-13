#include "boss_health.hpp"
#include "../conf/bullet.hpp"

#include "godot_cpp/classes/atlas_texture.hpp"
#include "godot_cpp/classes/resource_loader.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include <godot_cpp/classes/engine.hpp>

using namespace godot;
using namespace ui;

void BossHealth::_bind_methods() {}

BossHealth::BossHealth() {}

BossHealth::~BossHealth() {}

void BossHealth::_ready() {
  //  加载boss血条贴图
  ResourceLoader* res_loader = ResourceLoader::get_singleton();
  String out = conf::boos::health::path_outside.c_str();
  String in = conf::boos::health::path_inside.c_str();
  Ref<AtlasTexture> tex_over = Object::cast_to<AtlasTexture>(res_loader->load(out).ptr());
  Ref<AtlasTexture> tex_progress = Object::cast_to<AtlasTexture>(res_loader->load(in).ptr());

  // 将贴图应用到进度条
  if (tex_over.is_valid()) {
    set_over_texture(tex_over);
  }else {
    UtilityFunctions::print(out,"not found");
  }
  if (tex_progress.is_valid()) {
    set_progress_texture(tex_progress);
  }else {
    UtilityFunctions::print(in,"not found");
  }
  // 设置宽高
  set_custom_minimum_size(Vector2(193, 193));
  // 将锚点 和偏移 完全居中
  // PRESET_CENTER 代表正中央
  set_anchors_and_offsets_preset(LayoutPreset::PRESET_CENTER);

  // 将轴心点 也设置到自身大小的中心
  set_pivot_offset(get_size() / 2.0);
  set_fill_mode(FillMode::FILL_COUNTER_CLOCKWISE); // 顺时针填充
  set_z_index(90);  // 设置z索引
  set_nine_patch_stretch(true);
  // 初始化数值
  set_min(0.0);
  set_max(100.0); 
  set_value(100.0);
}

void BossHealth::update_health(double current_health, double max_health) {
  if (max_health <= 0.0) return;
  // 同步最大血量比例
  if (get_max() != max_health) {
    set_max(max_health);
  }
  set_value(current_health);
}