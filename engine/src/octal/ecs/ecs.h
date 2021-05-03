#pragma once
#include "octal/defines.h"
#include "octal/core/logger.h"
#include "octal/core/asserts.h"
#include "octal/ecs/components.h"
#include "octal/ecs/compstore.h"
#include <typeinfo>
#include <type_traits>
#include <deque>
#include <vector>


namespace octal {
  /// Alias Entity id to integer for separation of types
  typedef u32 EntityId;
  /// Entity Component Manager
  class ECS {
    private:

      /// Stores ids of unused entities
      std::deque<EntityId> m_EntityIds;

      /// The number of living entities right now
      u32 m_LivingEntities;

      /// Vector of component storage
      std::vector<Scope<CompStoreBase>> m_CompStorage;

    public:
      /// Constructor
      /// @param max maximum number of entities in this system
      ECS();

      /// Destructor
      ~ECS() {};
      
      /// Creates a new entity
      /// @return a fresh new unused EntityId
      EntityId CreateEntity();

      /// Destroys and entity
      /// @param id of entity that we no longer need anymore
      void DestroyEntity(EntityId id);

      /// Creates and adds a component to a given entity
      /// @param id of the entity we want to add the component to
      /// @param args arguments to the constructor of that component
      template<typename C, class... Args>
      void AddComponent(EntityId id, Args&&... args);

      /// removes a component from an entity
      /// @param id of the entity we want to remove the component from
      template<typename C>
      void RemoveComponent(EntityId id);

    private:
      /// Helper for creating or finding an component store
      template<typename C>
      Scope<CompStoreBase>& getComponentStore();
  };



  template<class T>
  // T is the base class. We need to do this to set the static s_Types variable.
  // we also use it to make sure that the types given are derived from T
  // May be useful if we need to do this with things other than components

  /// Helper to generate Component type ids
  class IdGenerator {
    private:
      /// The number of types we have
      static u8 s_Types;

    public:
      /// Creates a unique numerical representation of a type that is derived from Component
      /// @return a number representing the type given
      template<typename C>
      static const u8 TypeId() {
        // assert that C is derived from T
        constexpr bool is_derived = std::is_base_of<T, C>::value;
        STATIC_ASSERT(is_derived, "Attempt to check id of type that is not derived from Component");
        // create a new type if we can
        static const u8 tid = ++s_Types;
        return tid;
      };
  };
}
