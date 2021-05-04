#pragma once
#include "octal/ecs/scene.h"

namespace octal {

  /// A wrapper class over entity id's to make using Entities easier
  class Entity {
    private:
      friend Scene;
      /// The scene this entity belongs to
      /// Weak reference to scene since the scene owns our scope
      Scene *m_Scene;

      /// This entity's id!
      u32 m_id;

      /// No default constructor
      Entity() = delete;

      /// Create an entity by giving an id
      /// This can only be called by the scene
      /// @param id to give thie entity
      explicit Entity(u32 id) : m_id(id) {
      
      }

    public:

      /// Add component to this entity
      /// @param args the arguments to create that type of component
      template<typename... Args, typename C = std::common_type_t<Args...>>
      void AddComponent(Args&&... args) {
        m_Scene->m_ecs.AddComponent<C>(m_id, std::forward<C>(args...));
      }

      /// 
      template<typename C>
      void RemoveComponent() {
        m_Scene->m_ecs.RemoveComponent<C>(m_id);
      }

      template<typename C>
        // TODO: should be a reference counted pointer
      C* GetComponent() {
        return m_Scene->m_ecs.GetComponent<C>(m_id);
      }
  };
}
