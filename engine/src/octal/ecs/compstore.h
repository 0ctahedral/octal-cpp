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
    CompStoreBase() {};

    /// Virtual destructor
    virtual ~CompStoreBase() {};

    /// Notify the component store that an entity was destroyed
    virtual void EntityDestroyed(u32 id) = 0;
  };

  /// Template class for component storage
  /// This class stores components packed in memory so that we maintian
  /// cache locality
  template<typename C>
  class CompStore : public CompStoreBase {
    private:
      /// The maximum number entities we support
      const u32 m_Size;
      
      /// storage
      std::vector<C> m_Store;

      /// get the storage index of a component based on the entity id
      std::vector<i32> m_Id2Idx;
      
      /// the index of the last used component
      u32 m_LastIdx{0};

    public:
      CompStore(int max): m_Size(max){
        m_Store.resize(max);
        m_Id2Idx.resize(max);
        // set all to -1 to say invalid
        for (int i = 0; i < max; ++i) {
          m_Id2Idx[i] = -1;
        }
        // set all to negative 1
      };
      ~CompStore() override { m_Store.clear(); }


      /// Create a new component
      /// @param id the id of the entity to add
      /// @param args the arguments to the constructor of the entity
      template<class... Args>
      void Add(u32 id, Args&&... args) {
        // store the component in the closest available spot
        m_Store.emplace(m_Store.begin() + m_LastIdx, args...);
        // store the index of the component data at the index of the id
        m_Id2Idx[id] = m_LastIdx;
        // increase the last index
        ++m_LastIdx;
      }

      /// Get a reference to the component for this entity
      /// @param id of the entity whos component we are getting
      C& Get(u32 id) {
        //
        return nullptr;
      }

      /// Remove a component
      /// @param id the id of the entity to remove from
      void Remove(u32 id) {
        // if this is the last added component
        if (m_Id2Idx[id] == m_LastIdx - 1) {
          // we can just remove it from the end
          // and decrease the last idx counter
          --m_LastIdx;
          return;
        }
        // otherwise we need to:
        // clear the memory
        // swap the last idx with the empty one
        // update everything
        // decrease the last idx counter
        --m_LastIdx;
      }

      void EntityDestroyed(u32 id) override {

      }
  };
}
