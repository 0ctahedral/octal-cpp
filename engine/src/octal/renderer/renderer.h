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
      /// Create the instance
      /// @return false if the instance could not be created
      bool createInstance();
      /// Checks if Vulkan has the validation layers we require (currently hardcoded)
      bool hasValidationLayers();

      /// Setup the debugger for vulkan
      bool setupDebugMesenger();

      /// Our vulkan instance
      VkInstance m_Instance;
      /// Debug messenger for getting errors from vulkan
      VkDebugUtilsMessengerEXT m_Debugger;
      /// function that is called by the debugger
      static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
          VkDebugUtilsMessageSeverityFlagBitsEXT severity,
          VkDebugUtilsMessageTypeFlagsEXT msgType,
          const VkDebugUtilsMessengerCallbackDataEXT* data,
          void* userdata);
      /// Are the validation layers enabled? TODO: change this to using macros?
      bool validationEnabled = true;
  };

}
