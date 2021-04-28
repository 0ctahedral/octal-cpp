#pragma once
#include "octal/defines.h"
#include "octal/core/logger.h"
#include "octal/core/asserts.h"
#include "octal/ecs/components.h"
#include <typeinfo>
#include <type_traits>
#include <deque>

namespace octal {
  /// Alias Entity id to integer for separation of types
  typedef u32 EntityId;
  /// Entity Component Manager
  class ECS {
    private:
      /// Arbitrary limit to the number of entities
      const u32 MAX_ENTITIES;
      
      /// Stores ids of unused entities
      std::deque<EntityId> m_EntityIds;

      /// The number of living entities right now
      u32 m_LivingEntities;

    public:
      /// Constructor
      /// @param max maximum number of entities in this system
      ECS(u32 max = 5000);

      /// Destructor
      ~ECS() {};
      
      /// Creates a new entity
      /// @return a fresh new unused EntityId
      EntityId CreateEntity();

      /// Destroys and entity
      /// @param id of entity that we no longer need anymore
      void DestroyEntity(EntityId id);

      /// Adds a component to a given entity
      /*
      template<typename C>
      void AddComponent(EntityId id, C comp);
      */

    private:
      /// Creates a unique numerical representation of a type that is derived from Component
      /// @return a number representing the type given
      template<typename C>
      static constexpr std::size_t TypeId() {
        // assert that C is derived from Component
        constexpr bool is_derived = std::is_base_of<Component, C>::value;
        STATIC_ASSERT(is_derived, "Attempt to check id of type that is not derived from Component");
        return typeid(C).hash_code();
      };
  };
}
