#include "bullet_pool.hpp"

#include <godot_cpp/classes/physics_server2d.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/world2d.hpp> 
#include <godot_cpp/classes/physics_direct_space_state2d.hpp>
#include <godot_cpp/classes/physics_shape_query_parameters2d.hpp>

using namespace game;
using namespace godot;


BulletPool *BulletPool::new_bullet_pool = nullptr;

BulletPool *BulletPool::get_pool(){
  return new_bullet_pool;
}

void BulletPool::_bind_methods()
{
  // 注意：std::function 无法直接绑定给 Godot 的 ClassDB（GDScript 不支持 C++ 原生函数指针）
  // 如果需要从 GDScript 调用，建议在 C++ 内部预定义几种轨迹枚举，这里我们专注于纯 C++ 层的极致性能
}

BulletPool::BulletPool() {
    pool.resize(pool_size);
}

BulletPool::~BulletPool() {
  PhysicsServer2D *ps = PhysicsServer2D::get_singleton();
  RenderingServer *rs = RenderingServer::get_singleton();
  for (auto &b : pool) {
    if (b.area_rid.is_valid()) ps->free_rid(b.area_rid);
    if (b.shape_rid.is_valid()) ps->free_rid(b.shape_rid);
    if (b.canvas_item_rid.is_valid()) rs->free_rid(b.canvas_item_rid);
  }
}

void BulletPool::_ready() {
  new_bullet_pool = this;
  PhysicsServer2D *ps = PhysicsServer2D::get_singleton();
  RenderingServer *rs = RenderingServer::get_singleton();
  
  RID space_rid = get_world_2d()->get_space();
  RID canvas_rid = get_canvas();

  // 预初始化所有底层的物理和渲染容器（此时它们是空的“外壳”）
  for (size_t i = 0; i < pool_size; ++i) {
    // 1. 创建物理外壳
    pool[i].area_rid = ps->area_create();
    ps->area_set_space(pool[i].area_rid, space_rid);
    
    // 关键性能点：设置 4 层分层（我是玩家子弹层 3，只检测敌人层 2，不检测自身）
    ps->area_set_collision_layer(pool[i].area_rid, 4);
    ps->area_set_collision_mask(pool[i].area_rid, 2);
    ps->area_set_monitorable(pool[i].area_rid, false);

    // 创建一个碰撞形状外壳，后续动态修改其半径
    pool[i].shape_rid = ps->circle_shape_create();
    ps->area_add_shape(pool[i].area_rid, pool[i].shape_rid);

    // 2. 创建渲染外壳
    pool[i].canvas_item_rid = rs->canvas_item_create();
    rs->canvas_item_set_parent(pool[i].canvas_item_rid, canvas_rid);
    rs->canvas_item_set_visible(pool[i].canvas_item_rid, false);

    pool[i].active = false;
  }
}

// ... 构造、析构和 _ready 与之前保持一致 ...

void BulletPool::spawn(Vector2 p_pos, 
                        std::function<Vector2(const Bullet&, float)> p_behavior, 
                        float p_rot, 
                        Ref<Texture2D> p_tex, int p_h_frames, int p_v_frames, 
                        float p_radius, float p_anim_speed) {
  if (p_tex.is_null()) return;

  PhysicsServer2D *ps = PhysicsServer2D::get_singleton();
  RenderingServer *rs = RenderingServer::get_singleton();

  for (auto &b : pool) {
    if (!b.active) {
      b.position = p_pos;
      b.rotation = p_rot;
      b.lifetime = 0.0f; // 重置生命周期时间
      b.active = true;

      // 【核心修改】绑定行为函数指针
      b.behavior_fn = p_behavior;
      // 假设默认初始速度由函数在 time=0 时决定一次，或者给个默认值
      if (b.behavior_fn) {
          b.base_velocity = b.behavior_fn(b, 0.0f); 
          b.velocity = b.base_velocity;
      }

      b.config.texture = p_tex;
      b.config.h_frames = p_h_frames;
      b.config.v_frames = p_v_frames;
      b.config.radius = p_radius;
      b.config.anim_speed = p_anim_speed;
      b.anim_timer = 0.0f;
      b.current_frame = 0;

      ps->shape_set_data(b.shape_rid, p_radius);

      Transform2D xform(b.rotation, b.position);
      ps->area_set_transform(b.area_rid, xform);
      ps->area_set_monitorable(b.area_rid, true);
      rs->canvas_item_set_transform(b.canvas_item_rid, xform);
      rs->canvas_item_set_visible(b.canvas_item_rid, true);
      break;
    }
  }
}

void BulletPool::_physics_process(double delta){
  PhysicsServer2D *ps = PhysicsServer2D::get_singleton();
  RenderingServer *rs = RenderingServer::get_singleton();
  Vector2 screen_size = get_viewport_rect().size;
  for (auto &b : pool){
    if (!b.active)
      continue;
    // 累加子弹存活时间
    b.lifetime += delta;
    // 【核心修改】如果传入了行为函数指针，每帧动态计算速度
    if (b.behavior_fn)
    {
      b.velocity = b.behavior_fn(b, delta);
    }
    // 1. 位移计算
    b.position += b.velocity * delta;
    // 如果需要子弹朝向自适应运动方向，也可以在这里动态改旋转
    if (b.velocity != Vector2()){
      b.rotation = b.velocity.angle();
    }

    // 2. 边界判定与回收
    if (b.position.x < -50 || b.position.x > 952 + 50 ||
        b.position.y < -50 || b.position.y > 1062 + 50)
    {
      recycle_bullet(b);
      continue;
    }

    // 同步变换矩阵
    Transform2D xform(b.rotation, b.position);
    ps->area_set_transform(b.area_rid, xform);
    rs->canvas_item_set_transform(b.canvas_item_rid, xform);

    // ... 以下是原有的动画裁剪绘制、物理碰撞处理逻辑，完全保持不变 ...
     // 4. 【动态动画裁剪与刷新】
    rs->canvas_item_clear(b.canvas_item_rid); // 必须先清除前一帧的绘制命令
    
    // 累加时间更新当前帧
    b.anim_timer += delta;
    int total_frames = b.config.h_frames * b.config.v_frames;
    if (b.config.anim_speed > 0.0f) {
      b.current_frame = (int)(b.anim_timer * b.config.anim_speed) % total_frames;
    }

    // 根据当前帧，计算精灵图集（Texture）中的切片矩形 (Src Rect)
    Vector2 tex_size = b.config.texture->get_size();
    float frame_w = tex_size.x / b.config.h_frames;
    float frame_h = tex_size.y / b.config.v_frames;
    
    int frame_x = b.current_frame % b.config.h_frames;
    int frame_y = b.current_frame / b.config.h_frames;
    
    Rect2 src_rect(frame_x * frame_w, frame_y * frame_h, frame_w, frame_h); // 图集切片区域
    Rect2 dest_rect(-frame_w / 2.0f, -frame_h / 2.0f, frame_w, frame_h);   // 居中渲染目标区域

    // 将当前帧的贴图区域发送给渲染服务器
    rs->canvas_item_add_texture_rect_region(
      b.canvas_item_rid, 
      dest_rect, 
      b.config.texture->get_rid(), 
      src_rect
    );
    // 5. 【物理碰撞处理（修正版）】
    // 获取当前 2D 物理世界的直接空间状态指针
    PhysicsDirectSpaceState2D *space_state = get_world_2d()->get_direct_space_state();
    if (space_state != nullptr) {
      // 创建并配置碰撞形状查询参数
      Ref<PhysicsShapeQueryParameters2D> query;
      query.instantiate();
      
      query->set_shape_rid(b.shape_rid);         // 使用当前子弹的形状
      query->set_transform(xform);               // 使用当前子弹的位置和角度
      query->set_collision_mask(1);              // 仅扫描第 1 层（玩家本体）
      query->set_collide_with_areas(true);       // 允许撞击 Area2D
      query->set_collide_with_bodies(true);      // 允许撞击 CharacterBody2D 等物理体

      // 执行相交形状查询，返回最多 32 个碰撞结果（STG游戏通常撞到一个就可以回收了，32 绰绰有余）
      TypedArray<Dictionary> results = space_state->intersect_shape(query, 32);

      if (results.size() > 0) {
        for (int i = 0; i < results.size(); ++i) {
          Dictionary collision = results[i];
          // 从字典结果中提取碰撞对象的 ObjectID
          ObjectID obj_id = collision["collider_id"];
          Object *enemy = ObjectDB::get_instance(obj_id);
          if (enemy != nullptr) {
            // 触发敌人已有的减血或伤害函数
            enemy->call("take_damage", 10); 
          }
        }
        // 撞击后回收当前子弹
        recycle_bullet(b);
      }
    }
  }
}

void BulletPool::recycle_bullet(Bullet &b) {
    PhysicsServer2D *ps = PhysicsServer2D::get_singleton();
    RenderingServer *rs = RenderingServer::get_singleton();
    
    b.active = false;
    b.behavior_fn = nullptr; // 释放函数指针引用
    ps->area_set_monitorable(b.area_rid, false);
    rs->canvas_item_set_visible(b.canvas_item_rid, false);
    rs->canvas_item_clear(b.canvas_item_rid);
}
