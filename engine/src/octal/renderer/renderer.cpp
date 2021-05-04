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

  bool Renderer::hasValidationLayers() {
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
        return true;
      }
    }

    return false;
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
      extNames.emplace_back(ext.extensionName);
    }

    std::vector<const char*> layerNames = {"VK_LAYER_KHRONOS_validation"};

    VkInstanceCreateInfo create{};
    create.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create.pApplicationInfo = &app;
    create.enabledExtensionCount = extNames.size();
    create.ppEnabledExtensionNames = extNames.data();
    if (hasValidationLayers() && validationEnabled) {
      create.enabledLayerCount = layerNames.size();
      create.ppEnabledLayerNames = layerNames.data();
    } else {
      create.enabledLayerCount = 0;
    }

    auto result = vkCreateInstance(&create, nullptr, &m_Instance);
    if (result == VK_ERROR_EXTENSION_NOT_PRESENT) {
      FATAL("Could not find all extensions needed");
      return false;
    }
    return (result == VK_SUCCESS);
  }

}
