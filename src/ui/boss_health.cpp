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
  // // 如果是在编辑器中运行，直接返回，避免在编辑时修改属性
  // if (Engine::get_singleton()->is_editor_hint()) return;

  // 1. 获取资源加载器单例
  ResourceLoader* res_loader = ResourceLoader::get_singleton();

  // 2. 动态加载贴图资源 (请根据你项目的实际路径修改 "res://...")
  // 提示：C++ 中返回的是 Ref<Resource>，需要使用 cast_to 转换为 Ref<Texture2D>
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
  // 1. 将锚点（Anchors）和偏移（Offsets）完全居中
  // PRESET_CENTER 代表正中央
  set_anchors_and_offsets_preset(LayoutPreset::PRESET_CENTER);

  // 2. 将轴心点（Pivot）也设置到自身大小的中心
  // 这一步非常关键！如果不设置这个，血条在放大（Scale）时会往右下角偏移，而不是从中心向外放大
  set_pivot_offset(get_size() / 2.0);
  // 自动配置为圆形进度条模式
  set_fill_mode(FillMode::FILL_COUNTER_CLOCKWISE); // 顺时针填充（也可以选逆时针）
  // 设置z索引
  set_z_index(90);
  // 确保九宫格拉伸或者缩放正常（根据需求可选）
  set_nine_patch_stretch(true);
  // 初始化数值
  set_min(0.0);
  set_max(100.0); 
  set_value(100.0);
}

void BossHealth::update_health(double current_health, double max_health) {
  if (max_health <= 0.0) return;
  // 1. 动态同步最大血量比例
  if (get_max() != max_health) {
    set_max(max_health);
  }
  set_value(current_health);
}