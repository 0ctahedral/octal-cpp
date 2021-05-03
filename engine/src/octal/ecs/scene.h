#pragma once
#include "octal/ecs/ecs.h"
namespace octal {
  class Entity;
  /// Stores the ECS for the current scene
  class Scene {
    private:
      /// So that an entity can use the ecs of it's scene
      friend Entity;
      /// This scene's private ecs
      ECS m_ecs;
      
    public:
      Scene();
      ~Scene();

      Entity CreateEntity();
      void DestroyEntity(Entity e);
  };
}
