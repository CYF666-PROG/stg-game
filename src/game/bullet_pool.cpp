#include "bullet_pool.hpp"

#include <godot_cpp/classes/physics_server2d.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/world2d.hpp> 
#include <godot_cpp/classes/physics_direct_space_state2d.hpp>
#include <godot_cpp/classes/physics_shape_query_parameters2d.hpp>
#include <godot_cpp/classes/atlas_texture.hpp>

using namespace game;
using namespace godot;

BulletPool *BulletPool::new_bullet_pool = nullptr;

BulletPool *BulletPool::get_pool(){
    return new_bullet_pool;
}

void BulletPool::_bind_methods() {}

BulletPool::BulletPool() {
    pool.resize(pool_size);
}

BulletPool::~BulletPool() {
    PhysicsServer2D *ps = PhysicsServer2D::get_singleton();
    RenderingServer *rs = RenderingServer::get_singleton();
    for (auto &b : pool) {
        if (b.shape_rid.is_valid()) ps->free_rid(b.shape_rid);
        if (b.canvas_item_rid.is_valid()) rs->free_rid(b.canvas_item_rid);
    }
}

void BulletPool::_ready() {
    new_bullet_pool = this;
    PhysicsServer2D *ps = PhysicsServer2D::get_singleton();
    RenderingServer *rs = RenderingServer::get_singleton();
    
    RID canvas_rid = get_canvas();

    for (size_t i = 0; i < pool_size; ++i) {
        pool[i].shape_rid = ps->circle_shape_create();
        pool[i].canvas_item_rid = rs->canvas_item_create();
        rs->canvas_item_set_parent(pool[i].canvas_item_rid, canvas_rid);
        rs->canvas_item_set_visible(pool[i].canvas_item_rid, false);
        pool[i].active = false;
    }
}

void BulletPool::spawn(Vector2 p_pos, 
                       std::function<void(Bullet&)> p_behavior, 
                       float p_rot, 
                       Ref<SpriteFrames> p_sprite_frames, 
                       StringName p_anim_name,
                       float p_radius) {
    if (p_sprite_frames.is_null() || !p_sprite_frames->has_animation(p_anim_name)) return;

    PhysicsServer2D *ps = PhysicsServer2D::get_singleton();
    RenderingServer *rs = RenderingServer::get_singleton();

    for (auto &b : pool) {
        if (!b.active) {
            b.position = p_pos;
            b.rotation = p_rot;
            b.lifetime = 0.0f;
            b.active = true;

            // 绑定行为
            b.behavior_fn = p_behavior;
            if (b.behavior_fn) {
                b.behavior_fn(b);
                b.velocity = b.base_velocity;
            }

            // 缓存动画基础数据，避免每帧去查询字典
            b.config.sprite_frames = p_sprite_frames;
            b.config.anim_name = p_anim_name;
            b.config.radius = p_radius;
            b.config.total_frames = p_sprite_frames->get_frame_count(p_anim_name);
            b.config.anim_speed = p_sprite_frames->get_animation_speed(p_anim_name);
            b.config.loop = p_sprite_frames->get_animation_loop(p_anim_name);

            b.anim_timer = 0.0f;
            b.current_frame = -1; // 设为 -1 强制触发第一帧的渲染绘制

            ps->shape_set_data(b.shape_rid, p_radius);
            
            Transform2D xform(b.rotation, b.position);
            rs->canvas_item_set_transform(b.canvas_item_rid, xform);
            rs->canvas_item_set_visible(b.canvas_item_rid, true);
            break;
        }
    }
}

void BulletPool::spawn_static(Vector2 p_pos, 
                             std::function<void(Bullet&)> p_behavior, 
                             float p_rot, 
                             Ref<Texture2D> p_texture, 
                             float p_radius) {
    if (p_texture.is_null()) return;

    PhysicsServer2D *ps = PhysicsServer2D::get_singleton();
    RenderingServer *rs = RenderingServer::get_singleton();

    for (auto &b : pool) {
        if (!b.active) {
            b.position = p_pos;
            b.rotation = p_rot;
            b.lifetime = 0.0f;
            b.active = true;

            b.behavior_fn = p_behavior;
            if (b.behavior_fn) {
                b.behavior_fn(b);
                b.velocity = b.base_velocity;
            }

            // 清空动画相关缓存
            b.config.sprite_frames.unref(); 
            b.config.total_frames = 1;
            b.config.anim_speed = 0.0f;
            b.current_frame = 0; // 固定在第 0 帧

            ps->shape_set_data(b.shape_rid, p_radius);
            
            // 【核心绘制】因为是静态的，直接在这里 clear 并 add 一次，_physics_process 里就不用再动了！
            rs->canvas_item_clear(b.canvas_item_rid);
            
            Ref<AtlasTexture> atlas_tex = p_texture;
            if (atlas_tex.is_valid()) {
                Ref<Texture2D> main_texture = atlas_tex->get_atlas();
                Rect2 src_rect = atlas_tex->get_region();
                Rect2 dest_rect(-src_rect.size / 2.0f, src_rect.size);
                rs->canvas_item_add_texture_rect_region(b.canvas_item_rid, dest_rect, main_texture->get_rid(), src_rect);
            } else {
                Size2 tex_size = p_texture->get_size();
                Rect2 dest_rect(-tex_size / 2.0f, tex_size);
                rs->canvas_item_add_texture_rect(b.canvas_item_rid, dest_rect, p_texture->get_rid());
            }

            Transform2D xform(b.rotation, b.position);
            rs->canvas_item_set_transform(b.canvas_item_rid, xform);
            rs->canvas_item_set_visible(b.canvas_item_rid, true);
            break;
        }
    }
}

void BulletPool::_physics_process(double delta){
    PhysicsServer2D *ps = PhysicsServer2D::get_singleton();
    RenderingServer *rs = RenderingServer::get_singleton();

    Ref<PhysicsShapeQueryParameters2D> query;
    query.instantiate();
    query->set_collision_mask(1); 
    query->set_collide_with_areas(true); 
    query->set_collide_with_bodies(true); 

    PhysicsDirectSpaceState2D *space_state = get_world_2d()->get_direct_space_state();

    for (auto &b : pool){
        if (!b.active) continue;

        b.lifetime += delta;

        if (b.behavior_fn) {
            b.behavior_fn(b);
        }

        // 1. 位移
        b.position += b.velocity * delta;

        // 2. 边界判定
        if (b.position.x < -50 || b.position.x > 952 + 50 ||
            b.position.y < -50 || b.position.y > 1062 + 50)
        {
            recycle_bullet(b);
            continue;
        }

        Transform2D xform(b.rotation, b.position);
        rs->canvas_item_set_transform(b.canvas_item_rid, xform);

        // ==================== 【关键修改点就在这里】 ====================
        // 3. 【SpriteFrames 动画核心计算】—— 仅当包含动画资源时才运行
        if (b.config.sprite_frames.is_valid()) {
            b.anim_timer += delta;
            int next_frame = 0;
            
            if (b.config.anim_speed > 0.0f) {
                int calculated_frame = (int)(b.anim_timer * b.config.anim_speed);
                if (b.config.loop) {
                    next_frame = calculated_frame % b.config.total_frames;
                } else {
                    next_frame = MIN(calculated_frame, b.config.total_frames - 1);
                }
            }

            // 【优化点：脏检查】只有在帧发生切换时，才清空并重新提交渲染命令
            if (next_frame != b.current_frame) {
                b.current_frame = next_frame;
                rs->canvas_item_clear(b.canvas_item_rid);

                // 从 SpriteFrames 获取当前帧的 Texture2D 资源
                Ref<Texture2D> frame_tex = b.config.sprite_frames->get_frame_texture(b.config.anim_name, b.current_frame);
                
                if (frame_tex.is_valid()) {
                    Ref<AtlasTexture> atlas_tex = frame_tex;
                    
                    if (atlas_tex.is_valid()) {
                        // 如果 SpriteFrames 里塞的是 AtlasTexture (即大图切片)
                        Ref<Texture2D> main_texture = atlas_tex->get_atlas();
                        Rect2 src_rect = atlas_tex->get_region();
                        Rect2 dest_rect(-src_rect.size / 2.0f, src_rect.size);

                        rs->canvas_item_add_texture_rect_region(
                            b.canvas_item_rid, 
                            dest_rect, 
                            main_texture->get_rid(), 
                            src_rect
                        );
                    } else {
                        // 如果 SpriteFrames 里塞的是独立的普通 Texture
                        Size2 tex_size = frame_tex->get_size();
                        Rect2 dest_rect(-tex_size / 2.0f, tex_size);

                        rs->canvas_item_add_texture_rect(
                            b.canvas_item_rid, 
                            dest_rect, 
                            frame_tex->get_rid()
                        );
                    }
                }
            }
        }
        // =============================================================

        // 4. 物理碰撞
        if (space_state != nullptr) {
            query->set_shape_rid(b.shape_rid); 
            query->set_transform(xform); 

            TypedArray<Dictionary> results = space_state->intersect_shape(query, 32);
            if (results.size() > 0) {
                for (int i = 0; i < results.size(); ++i) {
                    Dictionary collision = results[i];
                    ObjectID obj_id = collision["collider_id"];
                    Object *enemy = ObjectDB::get_instance(obj_id);
                    if (enemy != nullptr) {
                        enemy->call("take_damage", 10); 
                    }
                }
                recycle_bullet(b);
            }
        }
    }
}

void BulletPool::recycle_bullet(Bullet &b) {
    RenderingServer *rs = RenderingServer::get_singleton();
    b.active = false;
    b.behavior_fn = nullptr; 
    b.config.sprite_frames.unref(); // 释放资源引用计数
    b.current_frame = -1;
    rs->canvas_item_set_visible(b.canvas_item_rid, false);
    rs->canvas_item_clear(b.canvas_item_rid);
}