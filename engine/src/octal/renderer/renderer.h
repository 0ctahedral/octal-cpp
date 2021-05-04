#pragma once
#include "octal/defines.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace octal {

  //TODO: Pimpl method or whatever
  /// A renderer. duh
  class Renderer {
    public:
      /// Constructor
      Renderer() {};
      /// Destructor
      ~Renderer() {};
      /// Initialize the renderer
      /// @return false if initializing fails
      bool Init();

      /// Cleans up resources used by the renderer and shuts it down
      void Shutdown();

    private:
      /// Our vulkan instance
      VkInstance m_Instance;
      /// Create the instance
      bool createInstance();
      bool hasValidationLayers();
      bool validationEnabled = true;
  };

}
