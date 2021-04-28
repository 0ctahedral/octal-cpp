#pragma once
#include "octal/defines.h"
#include "octal/ecs/components.h"
#include <vector>

namespace octal {

  /// Virtual class for which component storage is derived
  class CompStoreBase {
    public:
    /// Creates a component storage container
    /// @param max maximum number of entities
    CompStoreBase(int max);

    /// Virtual destructor
    virtual ~CompStoreBase() = 0;

    /// Notify the component store that an entity was destroyed
    virtual void EntityDestroyed(u32 id) = 0;
  };

  /// Template class for component storage
  template<typename C>
  class CompStore : public CompStoreBase {
    private:
      /// The maximum number entities we support
      const u32 m_Size;
      
      /// storage
      std::vector<C> m_Store;

    public:
      CompStore(int max): m_Size(max){
        m_Store.resize(max);
      };
      ~CompStore() { m_Store.erase(); }


      /// Create a new component
      /// @param id the id of the entity to add
      /// @param args the arguments to the constructor of the entity
      template<class... Args>
      void Add(u32 id, Args&&... args);

      /// Remove a component
      /// @param id the id of the entity to remove from
      template<class... Args>
      void Remove(u32 id);

      void EntityDestroyed(u32 id) override;
  };
}
