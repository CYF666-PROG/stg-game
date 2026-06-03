#include "entity.hpp"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/circle_shape2d.hpp>

using namespace game ;

void game::Entity::dead(){
  queue_free();
}

void game::Entity::entity_physics_process(double delta){}

void game::Entity::set_texture_coll(
    const godot::String path, 
    double d,
    godot::Vector2 scale
){
  // // 1. 获取资源加载器的单例
  // godot::ResourceLoader* loader = godot::ResourceLoader::get_singleton();
  // // 2. 直接加载资源并进行安全强转
  // godot::Ref<godot::Texture2D> texture = loader->load(path);
  // // 3. 检查是否加载成功
  // if (!texture.is_valid()) {
  //   godot::UtilityFunctions::print(path, " load erro");
  //   return;
  // }
  // /// 创建精灵并添加贴图
  // godot::Sprite2D* sprite2d = memnew(godot::Sprite2D) ;
  // sprite2d->set_texture(texture);
  // /// 设置缩放
  // sprite2d->set_scale(scale);
  // add_child(sprite2d);
  // 1. 使用 memnew 创建 CollisionShape2D 节点本身（容器）
  godot::CollisionShape2D* collision = memnew(godot::CollisionShape2D);
  // 2. 使用 memnew 创建具体的形状资源（核心数据）
  // 注意：Godot 的资源类在 C++ 中必须用 godot::Ref<> 智能指针包裹，否则会导致内存泄漏
  godot::Ref<godot::CircleShape2D> circle_resource = memnew(godot::CircleShape2D);
  // 3. 设置形状资源的物理属性
  circle_resource->set_radius(d);
  // 4. 将形状资源装载进节点容器中
  collision->set_shape(circle_resource);
  add_child(collision);
}

void game::Entity::set_texture_coll(
    const godot::Ref<godot::Texture2D> tex, 
    double d, 
    godot::Vector2 scale
){
  /// 创建精灵并添加贴图
  godot::Sprite2D* sprite2d = memnew(godot::Sprite2D) ;
  sprite2d->set_texture(tex);
  /// 设置缩放
  sprite2d->set_scale(scale);
  add_child(sprite2d);
  // 1. 使用 memnew 创建 CollisionShape2D 节点本身（容器）
  godot::CollisionShape2D* collision = memnew(godot::CollisionShape2D);
  // 2. 使用 memnew 创建具体的形状资源（核心数据）
  // 注意：Godot 的资源类在 C++ 中必须用 godot::Ref<> 智能指针包裹，否则会导致内存泄漏
  godot::Ref<godot::CircleShape2D> circle_resource = memnew(godot::CircleShape2D);
  // 3. 设置形状资源的物理属性
  circle_resource->set_radius(d);
  // 4. 将形状资源装载进节点容器中
  collision->set_shape(circle_resource);
  add_child(collision);
}

void Entity::_bind_methods() {}

void game::Entity::update_animation(){}

void game::Entity::_physics_process(double delta)
{
  ///检查是否处于编辑器
  if (godot::Engine::get_singleton()->is_editor_hint()){
    return;
  }
  entity_physics_process(delta);
}

void game::Entity::_ready(){}

Entity::Entity(){
  this->keyboard = input::KeyBoard::get_singleton();
}

Entity::~Entity(){
  this->keyboard = nullptr;
}