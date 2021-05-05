#pragma once
#include "octal/defines.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <optional>

namespace octal {
  /// Struct for storing QueueFamilies
  struct QueueFamilyIndices {
    std::optional<u32> graphicsFamily;
    bool isComplete() {
      return graphicsFamily.has_value();
    }
  };

  //TODO: Pimpl method or whatever
  /// A renderer. duh
  class Renderer {
    /// Our vulkan instance
    VkInstance m_Instance;

    /// Logical device we are using
    VkDevice m_Device;

    /// The physical device (graphics card) we are rendering with
    VkPhysicalDevice m_PhysicalDev{VK_NULL_HANDLE};

    /// Debug messenger for getting errors from vulkan
    VkDebugUtilsMessengerEXT m_Debugger;

    VkQueue m_GraphicsQ;

    QueueFamilyIndices m_QIndices;

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

      /// Finds and picks the ideal physical device
      bool pickPhysicalDevice(VkPhysicalDevice* pd);

      /// Creates our logical device
      bool createLogicalDevice();

      /// function that is called by the debugger
      static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
          VkDebugUtilsMessageSeverityFlagBitsEXT severity,
          VkDebugUtilsMessageTypeFlagsEXT msgType,
          const VkDebugUtilsMessengerCallbackDataEXT* data,
          void* userdata);
      /// Are the validation layers enabled? TODO: change this to using macros?
      bool validationEnabled = true;

      /// Find queues available for our use
      QueueFamilyIndices findQueueFamilies(VkPhysicalDevice dev);

  };

}
