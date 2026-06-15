#include "bullet_pool.hpp"
#include "entity.hpp"
#include "godot_cpp/classes/node.hpp"

#include <godot_cpp/classes/physics_server2d.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/world2d.hpp> 
#include <godot_cpp/classes/physics_direct_space_state2d.hpp>
#include <godot_cpp/classes/physics_shape_query_parameters2d.hpp>
#include <godot_cpp/classes/atlas_texture.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

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
  new_bullet_pool = nullptr;
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
    // 默认渲染层级 z_index
    rs->canvas_item_set_z_index(pool[i].canvas_item_rid, 10);
    rs->canvas_item_set_visible(pool[i].canvas_item_rid, false);
    pool[i].active = false;
  }
}

void BulletPool::spawn(
  godot::Vector2 p_pos, 
  std::function<void(Bullet&)> p_behavior, 
  godot::Ref<godot::SpriteFrames> p_sprite_frames, 
  godot::StringName p_anim_name,
  float p_radius,
  int mask,
  int z_index,
  godot::Vector2 p_scale,
  godot::Vector2 p_anchor,
  float p_rot,
  std::function<bool(godot::Node*)> collision_behavior
) {
  if (p_sprite_frames.is_null() || !p_sprite_frames->has_animation(p_anim_name)) return;

  PhysicsServer2D *ps = PhysicsServer2D::get_singleton();
  RenderingServer *rs = RenderingServer::get_singleton();

  for (auto &b : pool) {
    if (!b.active) {
      b.position = p_pos;
      b.rotation = p_rot;
      b.lifetime = 0.0f;
      b.active = true;
      /// 设置碰撞遮罩
      b.mask = mask;
      /// 设置z轴索引
      rs->canvas_item_set_z_index(b.canvas_item_rid, z_index);
      // 绑定lambda
      b.collision_behavior = collision_behavior;
      b.behavior_fn = p_behavior;
      if (b.behavior_fn) {
        b.behavior_fn(b);
      }
      // 缓存新增的缩放和锚点
      b.config.scale = p_scale;
      b.config.anchor = p_anchor;
      // 缓存动画基础数据
      b.config.sprite_frames = p_sprite_frames;
      b.config.anim_name = p_anim_name;
      b.config.radius = p_radius;
      b.config.total_frames = p_sprite_frames->get_frame_count(p_anim_name);
      b.config.anim_speed = p_sprite_frames->get_animation_speed(p_anim_name);
      b.config.loop = p_sprite_frames->get_animation_loop(p_anim_name);
      // 设置子弹绘制层级 防止子弹重叠时抽搐/鬼畜
      size_t index = &b - &pool[0]; 
      rs->canvas_item_set_draw_index(b.canvas_item_rid, index);

      b.anim_timer = 0.0f;
      b.current_frame = -1; // 设为 -1 则强制触发第一帧的渲染绘制

      ps->shape_set_data(b.shape_rid, p_radius);
      
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
  query->set_collide_with_areas(true); 
  query->set_collide_with_bodies(true); 

  PhysicsDirectSpaceState2D *space_state = get_world_2d()->get_direct_space_state();

  for (auto &b : pool){
    if (!b.active) continue;

    if (b.behavior_fn) {
      b.behavior_fn(b);
    }

    b.lifetime++;

    // 位移
    b.position += b.velocity;

    // 出界判定
    if (b.position.x < -50 || b.position.x > 952 + 50 ||
      b.position.y < -50 || b.position.y > 1062 + 50)
    {
      recycle_bullet(b);
      continue;
    }

    Transform2D xform(b.rotation, b.position);
    rs->canvas_item_set_transform(b.canvas_item_rid, xform);

    // 仅当包含动画资源时才运行
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

      // 只有在帧发生切换时 才清空并重新提交渲染命令
      if (next_frame != b.current_frame) {
        b.current_frame = next_frame;
        rs->canvas_item_clear(b.canvas_item_rid);

        // 从 SpriteFrames 获取当前帧的 Texture2D 资源
        Ref<Texture2D> frame_tex = b.config.sprite_frames->get_frame_texture(b.config.anim_name, b.current_frame);
        
        if (frame_tex.is_valid()) {
          Ref<AtlasTexture> atlas_tex = frame_tex;
          
          // 提取缩放和锚点到局部变量
          Vector2 scale = b.config.scale;
          Vector2 anchor = b.config.anchor;

          if (atlas_tex.is_valid()) {
            // AtlasTexture 逻辑
            Ref<Texture2D> main_texture = atlas_tex->get_atlas();
            Rect2 src_rect = atlas_tex->get_region();
            // 根据缩放计算实际渲染大小
            Size2 custom_size = src_rect.size * scale;
            // 根据锚点计算左上角偏移
            Vector2 offset = -custom_size * anchor;
            Rect2 dest_rect(offset, custom_size);

            rs->canvas_item_add_texture_rect_region(
                b.canvas_item_rid, 
                dest_rect, 
                main_texture->get_rid(), 
                src_rect
            );
          } else {
            // 普通 Texture
            Size2 tex_size = frame_tex->get_size();
            // 根据缩放计算实际渲染大小
            Size2 custom_size = tex_size * scale;
            // 根据锚点计算左上角偏移
            Vector2 offset = -custom_size * anchor;
            Rect2 dest_rect(offset, custom_size);
            rs->canvas_item_add_texture_rect(
              b.canvas_item_rid, 
              dest_rect, 
              frame_tex->get_rid()
            );
          }
        }
      }
    }else {
      // 静态单图逻辑
      if (b.current_frame == -1) {
        b.current_frame = 0; 
      }
    }
    // 物理碰撞
    // 设置碰撞遮罩
    query->set_collision_mask(b.mask); 
    if (space_state != nullptr) {
      query->set_shape_rid(b.shape_rid); 
      query->set_transform(xform); 

      TypedArray<Dictionary> results = space_state->intersect_shape(query, 32);
      if (results.size() > 0){
        bool is_tu_dead = false;
        for (int i = 0; i < results.size(); ++i) {
          Dictionary collision = results[i];
          ObjectID obj_id = collision["collider_id"];
          Object *enemy = ObjectDB::get_instance(obj_id);
          if (b.collision_behavior) {
            auto node = Object::cast_to<Node>(enemy);
            if (!node) continue;
            is_tu_dead = b.collision_behavior(node);
          }else {
            auto entity = Object::cast_to<game::Entity>(enemy);
            if (entity != nullptr) {
              entity->hit_bullet(); 
            }
            is_tu_dead = true;
          }
        }
        if(is_tu_dead) recycle_bullet(b);
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