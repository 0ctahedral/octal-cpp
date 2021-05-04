#include "octal/ecs/entity.h"

namespace octal {

  /*
      template<typename... Args, typename C>
      void Entity::AddComponent(Args&&... args) {
        m_Scene->m_ecs.AddComponent<C>(m_id, std::forward<C>(args...));
      }

      template<typename C>
      void Entity::RemoveComponent() {
        m_Scene->m_ecs.RemoveComponent<C>(m_id);
      }

      template<typename C>
      C* Entity::GetComponent() {
        return m_Scene->m_ecs.GetComponent<C>(m_id);
      }
      */
}
