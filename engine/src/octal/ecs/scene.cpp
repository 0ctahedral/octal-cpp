#include "octal/ecs/scene.h"
#include "octal/ecs/entity.h"

namespace octal {

  Entity Scene::CreateEntity() {
    return Entity(m_ecs.CreateEntity(), this);
  }

  void Scene::DestroyEntity(Entity e) {
    m_ecs.DestroyEntity(e.m_id);
  }
}
