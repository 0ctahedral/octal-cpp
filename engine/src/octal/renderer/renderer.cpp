#include "octal/renderer/renderer.h"
#include "octal/core/logger.h"
#include <vector>

namespace octal {
  bool Renderer::Init() {
    if (!createInstance()) {
      FATAL("Failed to create vk instance");
      return false;
    }


    return true;
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
    INFO("Vulkan has found %d extensions:", extCount);

    // get the extension props
    std::vector<VkExtensionProperties> extensions(extCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extCount, extensions.data());

    std::vector<const char*> names;

    // print and get names
    for (auto& ext : extensions) {
      INFO("  %s", ext.extensionName);
      names.push_back(ext.extensionName);
    }


    VkInstanceCreateInfo create{};
    create.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create.pApplicationInfo = &app;
    create.enabledExtensionCount = names.size();
    create.ppEnabledExtensionNames = names.data();
    // TODO: add some layers and shit
    create.enabledLayerCount = 0;
    create.ppEnabledLayerNames = nullptr;

    return vkCreateInstance(&create, nullptr, &m_Instance) == VK_SUCCESS;
  }
}
