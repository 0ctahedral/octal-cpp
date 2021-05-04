#include "octal/renderer/renderer.h"
#include "octal/core/logger.h"
#include <cstring>

namespace octal {
  bool Renderer::Init() {
    if (!createInstance()) {
      FATAL("Failed to create vk instance");
      return false;
    }
    return true;
  }

  void Renderer::Shutdown() {
    vkDestroyInstance(m_Instance, nullptr);
  }

  // morgan's little helpers
  std::vector<const char*> Renderer::getRequiredExtensions() {
    // get supported extension count
    u32 extCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);
    INFO("Vulkan supports %d extensions:", extCount);

    // get the extension props
    std::vector<VkExtensionProperties> extensions(extCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extCount, extensions.data());

    std::vector<const char*> extNames;
    // print and get names
    for (auto& ext : extensions) {
      INFO("  %s", ext.extensionName);
      extNames.push_back(ext.extensionName);
    }

    // if we are using the validation layer then we need to add the debug extension

    return extNames;
  }

  std::vector<const char*> Renderer::getValidationLayers() {
    u32 layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> layers(layerCount);
    INFO("Vulkan has found %d layers:", layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, layers.data());
    std::vector<const char*> layerNames;
    for (auto& lay : layers) {
      INFO("  %s", lay.layerName);
      // TODO: move the validation we want to an array
      // for now we only want the validation layer
      if (strcmp("VK_LAYER_KHRONOS_validation", lay.layerName) == 0) {
        layerNames.push_back(lay.layerName);
      }
    }

    return layerNames;
  }

  bool Renderer::createInstance() {
    // TODO: get name from application
    // Create application info
    VkApplicationInfo app{};
    app.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app.pApplicationName = "Test";
    app.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app.pEngineName = "octal";
    app.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app.apiVersion = VK_API_VERSION_1_2;

    std::vector<const char*> extensions = getRequiredExtensions();
    std::vector<const char*> layers = getValidationLayers();
    /*
    std::vector<const char*> extensions = {
      "VK_KHR_device_group_creation",
      "VK_KHR_display",
      "VK_KHR_external_fence_capabilities",
      "VK_KHR_external_memory_capabilitie",
      "VK_KHR_external_semaphore_capabilities",
      "VK_KHR_get_display_properties2",
      "VK_KHR_get_physical_device_properties2",
      "VK_KHR_get_surface_capabilities2",
      "VK_KHR_surface",
      "VK_KHR_surface_protected_capabilities",
     // "VK_KHR_wayland_surface",
      "VK_KHR_xcb_surface",
     // "VK_KHR_xlib_surface",
     // "VK_EXT_acquire_xlib_display",
     // "VK_EXT_debug_report",
     // "VK_EXT_direct_mode_display",
     // "VK_EXT_display_surface_counter",
     // "VK_EXT_debug_utils",
    };
    std::vector<const char*> layers = {
    //  "VK_LAYER_LUNARG_device_simulation",
    //  "VK_LAYER_LUNARG_monitor",
    //  "VK_LAYER_LUNARG_api_dump",
    //  "VK_LAYER_LUNARG_screenshot",
      "VK_LAYER_KHRONOS_validation",
    };
    */

    VkInstanceCreateInfo create{};
    create.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create.pApplicationInfo = &app;
    create.enabledExtensionCount = extensions.size();
    create.ppEnabledExtensionNames = extensions.data();
    create.enabledLayerCount = 0;
    create.ppEnabledLayerNames = nullptr;
    /*
    create.enabledLayerCount = layers.size();
    create.ppEnabledLayerNames = layers.data();
    */

    auto result = vkCreateInstance(&create, nullptr, &m_Instance);
    if (result == VK_ERROR_EXTENSION_NOT_PRESENT) {
      FATAL("Could not find all extensions needed");
      return false;
    }
    return (result == VK_SUCCESS);
  }

}
