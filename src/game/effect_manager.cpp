#include "effect_manager.hpp"
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/atlas_texture.hpp>
#include <godot_cpp/classes/engine.hpp>

using namespace game;
using namespace godot;

EffectManager *EffectManager::singleton = nullptr;

static inline void _rs_update_item_texture(RenderingServer *rs, RID canvas_item, const Ref<Texture2D> &tex) {
    if (tex.is_null()) return;
    rs->canvas_item_clear(canvas_item);

    Ref<AtlasTexture> atlas_tex = tex;
    if (atlas_tex.is_valid() && atlas_tex->get_atlas().is_valid()) {
        RID atlas_rid = atlas_tex->get_atlas()->get_rid();
        Rect2 src_region = atlas_tex->get_region();
        Vector2 size = src_region.size;
        rs->canvas_item_add_texture_rect_region(canvas_item, Rect2(-size / 2.0, size), atlas_rid, src_region);
    } else {
        rs->canvas_item_add_texture_rect(canvas_item, Rect2(-tex->get_size() / 2.0, tex->get_size()), tex->get_rid());
    }
}

void EffectManager::_bind_methods() {}

void EffectManager::_ready(){
  singleton = this;
}

EffectManager::EffectManager() {}

EffectManager::~EffectManager() {
    if (singleton == this) { singleton = nullptr; }
    RenderingServer *rs = RenderingServer::get_singleton();
    for (int i = 0; i < active_effects.size(); ++i) {
        if (active_effects[i].canvas_item.is_valid()) {
            rs->free_rid(active_effects[i].canvas_item);
        }
    }
}

void EffectManager::spawn_effect(
  const Ref<SpriteFrames> &p_frames, 
  Vector2 p_global_pos, 
  StringName p_anim_name, 
  double p_speed_scale, 
  EffectUpdateCallback p_callback,
  int z_index
) {
    if (p_frames.is_null() || !p_frames->has_animation(p_anim_name)) return;

    RenderingServer *rs = RenderingServer::get_singleton();
    RID ci = rs->canvas_item_create();
    rs->canvas_item_set_parent(ci, get_canvas_item());
    // 设置z轴索引
    rs->canvas_item_set_z_index(ci, z_index);
    // 默认变换 (旋转0, 缩放1, 原点是初始位置)
    Transform2D xform;
    xform.set_origin(p_global_pos);
    rs->canvas_item_set_transform(ci, xform);

    _rs_update_item_texture(rs, ci, p_frames->get_frame_texture(p_anim_name, 0));

    int engine_fps = Engine::get_singleton()->get_physics_ticks_per_second(); 
    double anim_fps = p_frames->get_animation_speed(p_anim_name) * p_speed_scale;
    if (anim_fps <= 0.0) anim_fps = 1.0;

    int ticks_needed = Math::round((double)engine_fps / anim_fps);
    if (ticks_needed < 1) ticks_needed = 1;

    EffectInstance inst;
    inst.canvas_item = ci;
    inst.sprite_frames = p_frames;
    inst.anim_name = p_anim_name;
    inst.base_position = p_global_pos; // 记录初始位置
    
    // 初始化其他数值为默认
    inst.current_frame = 0;
    inst.tick_counter = 0;
    inst.total_elapsed_ticks = 0;
    inst.ticks_per_frame = ticks_needed;
    inst.loop = p_frames->get_animation_loop(p_anim_name);
    inst.update_callback = p_callback;

    active_effects.push_back(inst);
}

void EffectManager::_physics_process(double delta) {
    RenderingServer *rs = RenderingServer::get_singleton();
    
    EffectInstance *effects_ptr = active_effects.ptrw();
    int effect_count = active_effects.size();

    for (int i = effect_count - 1; i >= 0; --i) {
        EffectInstance &fx = effects_ptr[i];

        if (fx.sprite_frames.is_null() || !fx.sprite_frames->has_animation(fx.anim_name)) {
            rs->free_rid(fx.canvas_item);
            active_effects.remove_at(i);
            effects_ptr = active_effects.ptrw();
            continue;
        }

        fx.total_elapsed_ticks++;

        // 记录状态用于比对
        int frame_before_lambda = fx.current_frame;
        StringName anim_before_lambda = fx.anim_name;
        
        // --- 核心变化 1：在 Lambda 调用前，无需计算 Transform，用户只需专注于数值 ---

        // --- 执行 Lambda 每帧动态回调 ---
        if (fx.update_callback != nullptr) {
            bool keep_alive = fx.update_callback(fx, fx.total_elapsed_ticks);
            if (!keep_alive) {
                rs->free_rid(fx.canvas_item);
                active_effects.remove_at(i);
                effects_ptr = active_effects.ptrw();
                continue; 
            }

            // --- 核心变化 2：经理自动将 Lambda 修改的 rotation/scale 应用给底层渲染 ---
            Transform2D xform;
            // 自动合成变换：先按原点缩放、旋转，最后偏移到 base_position
            xform.set_rotation_and_scale(fx.rotation, fx.scale);
            xform.set_origin(fx.base_position);
            
            rs->canvas_item_set_transform(fx.canvas_item, xform);
            rs->canvas_item_set_modulate(fx.canvas_item, fx.modulate); // 自动改颜色

            // 手动切帧处理依然保留
            if (fx.current_frame != frame_before_lambda || fx.anim_name != anim_before_lambda) {
                Ref<Texture2D> manual_tex = fx.sprite_frames->get_frame_texture(fx.anim_name, fx.current_frame);
                _rs_update_item_texture(rs, fx.canvas_item, manual_tex);
            }
        }

        // --- 正常的自动化逻辑帧计时与切换 ---
        fx.tick_counter++;

        if (fx.tick_counter >= fx.ticks_per_frame) {
            fx.tick_counter = 0;
            fx.current_frame++;

            int total_frames = fx.sprite_frames->get_frame_count(fx.anim_name);

            if (fx.current_frame >= total_frames) {
                if (fx.loop) {
                    fx.current_frame = 0;
                } else {
                    rs->free_rid(fx.canvas_item);
                    active_effects.remove_at(i);
                    effects_ptr = active_effects.ptrw();
                    continue; 
                }
            }

            Ref<Texture2D> next_tex = fx.sprite_frames->get_frame_texture(fx.anim_name, fx.current_frame);
            _rs_update_item_texture(rs, fx.canvas_item, next_tex);
        }
    }
}