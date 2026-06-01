#include "bullet_pool.hpp"

using namespace game;


bullet::RoundBullet *game::BulletPool::get_bullet(){
  // 1. 寻找池中目前处于闲置状态的子弹
  for (bullet::RoundBullet* bullet : bullets) {
    if (!bullet->is_visible) { // 判断是否闲置
      bullet->is_visible = true; // 标记为正在使用
      godot::UtilityFunctions::print("get_bullet");
      return bullet;
    }
  }
  // 2. 如果池子满了，动态扩容
  godot::UtilityFunctions::print("Bullet pool exhausted! Expanding...");
  bullet::RoundBullet* new_bullet = memnew(bullet::RoundBullet);
  new_bullet->deactivate();
  bullets.push_back(new_bullet);
  return new_bullet;
}
void game::BulletPool::_bind_methods(){}

BulletPool *game::BulletPool::get_pool(){
  return bullet_pool;
}

BulletPool::BulletPool(){}

BulletPool::~BulletPool(){
  // 内存手工清理（因为这些内存没在场景树里时，Godot不会自动释放它们）
  for (bullet::RoundBullet* bullet : bullets) {
    if (bullet && !bullet->is_inside_tree()) {
      memdelete(bullet); 
    }
  }
  bullets.clear();
}

void game::BulletPool::_ready(){
  bullet_pool = this ;
  // 预先实例化一定数量的子弹
  for (int i = 0; i < INITIAL_POOL_SIZE; ++i) {
    auto* bullet = memnew(bullet::RoundBullet);
    bullet->deactivate(); // 默认关闭
    bullets.push_back(bullet);
  }
}