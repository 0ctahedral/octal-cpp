#include "scene.h"
#include "entity.h"

namespace octal {

  Entity Scene::CreateEntity() {
    return Entity(m_ecs.CreateEntity());
  }
}
