#pragma once
#include "octal/defines.h"
#include "octal/core/logger.h"
#include "octal/core/asserts.h"
#include "octal/ecs/components.h"
#include <typeinfo>
#include <type_traits>

namespace octal {
  /// Entity Component Manager
  class ECS {
    private:
      static u8 s_Types;
    public:
      /// Constructor
      ECS() {};
      /// Destructor
      ~ECS() {};

      // start with just printing type ids
      template<typename C>
        void AddComponent(C comp) {
        }


    //private:
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
