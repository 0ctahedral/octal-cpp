#pragma once
#include "octal/defines.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <optional>

namespace octal {
  /// Struct for storing QueueFamilies
  struct QueueFamilyIndices {
    std::optional<u32> graphics;
    std::optional<u32> present;
    bool isComplete() {
      return graphics.has_value() && present.has_value();
    }
  };

  struct SwapchainDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> modes;
  };

  //TODO: Pimpl method or whatever
  /// A renderer. duh
  class Renderer {
    /// Our vulkan instance
    VkInstance m_Instance;

    /// Logical device we are using
    VkDevice m_Device;

    std::vector<const char*> m_DeviceExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    /// The physical device (graphics card) we are rendering with
    VkPhysicalDevice m_PhysicalDev{VK_NULL_HANDLE};

    /// Debug messenger for getting errors from vulkan
    VkDebugUtilsMessengerEXT m_Debugger;

    /// Graphics queue for our device
    VkQueue m_GraphicsQ;

    /// Presentaion queue for our device
    VkQueue m_PresentQ;

    /// Indices of the queues on the device
    QueueFamilyIndices m_QIndices;

    /// The surface we are rendering to
    VkSurfaceKHR m_Surface;

    /// The swapchain we are presenting with
    VkSwapchainKHR m_SwapChain;

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

      /// Create the surface
      /// @return if creating the surface was successful
      bool createSurface();

      /// Create the swapchain
      /// @return if creating the swapchain was successful
      bool createSwapChain();

      /// Get what the swapchain supports
      /// @param dev device we are querying on
      /// @param surface surface we are querying on
      SwapchainDetails querySwapchainSupport(VkPhysicalDevice dev, VkSurfaceKHR surface);

      /// Helper function
      VkPresentModeKHR chooseMode(const std::vector<VkPresentModeKHR>& modes);
      VkSurfaceFormatKHR chooseFormat(const std::vector<VkSurfaceFormatKHR>& formats);
      VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR& capabilities);
  };

}
