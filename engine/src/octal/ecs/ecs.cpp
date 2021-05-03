#include "octal/ecs/ecs.h"
/*
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

  u32 ECS::CreateEntity() {
    // make sure we can even create a new entity
    ASSERT(m_LivingEntities < MAX_ENTITIES, "Max entities exceeded");
    // set our return value to the frontmost entity id
    u32 ret = m_EntityIds.front();
    // pop the id off the front
    m_EntityIds.pop_front();
    // increase number of living entities
    ++m_LivingEntities;
    return ret;
  }

  /// Destroys and entity
  /// @param id of entity that we no longer need anymore
  void ECS::DestroyEntity(u32 id) {
    ASSERT(id < MAX_ENTITIES, "Invalid entity id given");
    // check if this entity has already been returned
    auto itr = std::find(m_EntityIds.begin(), m_EntityIds.end(), id);
    if (itr != m_EntityIds.end()) {
      WARN("Entity already %d destroyed! Skipping...", id);
      return;
    }
    // remove all the entity's components
    for (auto& itr : m_CompStorage) {
      itr->EntityDestroyed(id);
    }
    // add to back of list
    m_EntityIds.push_back(id);
    // reduce number of living entities
    --m_LivingEntities;
  }


  template<typename C, typename... Args>
  void ECS::AddComponent(u32 id, Args&&... args) {
    auto cs = getComponentStore<C>();
    cs.template Add<C>(id, std::forward<C>(args...));
  }

  template<typename C>
  void ECS::RemoveComponent(u32 id) {
    auto cs = getComponentStore<C>();
    cs.Remove(id);
  }

  template<typename C>
  C* ECS::GetComponent(u32 id) {
    auto cs = getComponentStore<C>();
    return cs.Get(id);
  }


  template<typename C>
  CompStore<C>& ECS::getComponentStore() {
    u8 tid = IdGenerator<Component>::TypeId<C>();

    // if the length of the comp storage vector is less than our id then that means we gotta add it
    if (tid < m_CompStorage.size()) {
      m_CompStorage.emplace_back(CompStore<C>());
    }

    return m_CompStorage[tid];
  }

}
*/
