#include "octal/defines.h"
#include "octal/ecs/ecs.h"
#include <algorithm>
namespace octal {

  ECS::ECS()
    :m_LivingEntities(0)
  { 
    // populate the queue
    // id of 0 is reserved for null
    for (int i = 1; i < MAX_ENTITIES; ++i) {
      m_EntityIds.push_back(i);
    }
  };

  EntityId ECS::CreateEntity() {
    // make sure we can even create a new entity
    ASSERT(m_LivingEntities < MAX_ENTITIES, "Max entities exceeded");
    // set our return value to the frontmost entity id
    EntityId ret = m_EntityIds.front();
    // pop the id off the front
    m_EntityIds.pop_front();
    // increase number of living entities
    ++m_LivingEntities;
    return ret;
  }

  /// Destroys and entity
  /// @param id of entity that we no longer need anymore
  void ECS::DestroyEntity(EntityId id) {
    ASSERT(id < MAX_ENTITIES, "Invalid entity id given");
    // check if this entity has already been returned
    auto itr = std::find(m_EntityIds.begin(), m_EntityIds.end(), id);
    if (itr != m_EntityIds.end()) {
      WARN("Entity already %d destroyed! Skipping...", id);
      return;
    }
    // add to back of list
    m_EntityIds.push_back(id);
    // reduce number of living entities
    --m_LivingEntities;
  }


  template<typename C, class... Args>
  void ECS::AddComponent(EntityId id, Args&&... args) {

  }

  template<typename C>
  void ECS::RemoveComponent(EntityId id) {

  }

  template<typename C>
  Scope<CompStoreBase>& ECS::getComponentStore() {
    u8 tid = IdGenerator<Component>::TypeId<C>();

    // if the length of the comp storage vector is less than our id then that means we gotta add it
    if (tid < m_CompStorage.size()) {
      m_CompStorage.emplace_back(CreateUni<CompStore<C>>(MAX_ENTITIES));
    }

    return m_CompStorage[tid];
  }

}
