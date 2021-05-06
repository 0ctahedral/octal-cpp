#include "platform/platform.h"
#include "platform/linux/linux.h"
#include "octal/renderer/renderer.h"
#include "octal/core/logger.h"
#include <cstring>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_xcb.h>

namespace octal {
  bool Renderer::Init() {
    if (!createInstance()) {
      FATAL("Failed to create vk instance");
      return false;
    }
    setupDebugMesenger();
    if (!pickPhysicalDevice(&m_PhysicalDev)) {
      FATAL("Failed to find suitable physical device");
      return false;
    }
    if (!createLogicalDevice()) {
      FATAL("Failed to create logical device");
      return false;
    }

    // get the device queue
    vkGetDeviceQueue(m_Device, m_QIndices.graphics.value(), 0, &m_GraphicsQ);


    if (!createSurface()) {
      FATAL("Failed to create surface");
      return false;
    }

    return true;
  }

  void Renderer::Shutdown() {
    vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
    // destroy the logical device
    vkDestroyDevice(m_Device, nullptr);
    // shutdown the debugger
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)
      vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func == nullptr) {
      ERROR("Could not find function 'vkDestroyDebugUtilsMessengerEXT'");
    }
    func(m_Instance, m_Debugger, nullptr);

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

    // TODO: limit this to only extensions we will need
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

      // add debugging layers
      VkDebugUtilsMessengerCreateInfoEXT debug{};
      debug.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
      debug.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
        |VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
      debug.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
        |VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
      debug.pfnUserCallback = debugCallback;
      create.pNext =(VkDebugUtilsMessengerCreateInfoEXT*) &debug;
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

  /// Callback for vulkan debugging
  VKAPI_ATTR VkBool32 VKAPI_CALL Renderer::debugCallback(
      VkDebugUtilsMessageSeverityFlagBitsEXT severity,
      VkDebugUtilsMessageTypeFlagsEXT msgType,
      const VkDebugUtilsMessengerCallbackDataEXT* data,
      void* userdata) {

    WARN("%s", data->pMessage);

    return VK_FALSE;
  }

  bool Renderer::setupDebugMesenger() {
    VkDebugUtilsMessengerCreateInfoEXT create{};
    create.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    /*
    create.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
      | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
      |VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    */
    create.messageSeverity =  VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
      |VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
      | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
      |VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create.pfnUserCallback = debugCallback;
    // get the function to set this bad boy up
    // it's an extension so we need to look it up
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)
      vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT");

    if (func == nullptr) {
      ERROR("Could not find function 'vkCreateDebugUtilsMessengerEXT'");
      return false;
    }

    return func(m_Instance, &create, nullptr, &m_Debugger) == VK_SUCCESS;
  }


  // TODO: take in a list of requirements
  bool Renderer::pickPhysicalDevice(VkPhysicalDevice* pd) {
    // get how many devices we have
    u32 deviceCount = 0;
    vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
      FATAL("Could not find any physical devices :(");
      return false;
    }

    // create and populate vector of devices
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

    // Check suitability of devices
    for (auto& dev : devices) {
      VkPhysicalDeviceProperties props;
      vkGetPhysicalDeviceProperties(dev, &props);
      VkPhysicalDeviceFeatures features;
      vkGetPhysicalDeviceFeatures(dev, &features);

      // print out the devices cuz why not
      INFO("Device %s: type=%d", props.deviceName, props.deviceType);

      m_QIndices = findQueueFamilies(dev);
      // TODO: select best device if we have more than one
      // ex: prefer descrete over integrated graphics
      if (features.geometryShader && m_QIndices.isComplete()) {
        *pd = dev;
        return true;
      }
    }

    return false; 
  }


  QueueFamilyIndices Renderer::findQueueFamilies(VkPhysicalDevice dev) {
    QueueFamilyIndices indices;

    u32 queueCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(dev, &queueCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueCount);

    vkGetPhysicalDeviceQueueFamilyProperties(dev, &queueCount, queueFamilies.data());

    for (u32 i = 0; i < queueCount; ++i) {
      if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        indices.graphics = i;
      }
    }

    return indices;
  }

  bool Renderer::createLogicalDevice() {
    // creates the device queue
    VkDeviceQueueCreateInfo queueCreate{};
    queueCreate.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    // set graphics index
    queueCreate.queueFamilyIndex = m_QIndices.graphics.value();
    queueCreate.queueCount = 1;
    // make this the highest priority
    float priority = 1.f;
    queueCreate.pQueuePriorities = &priority;

    // device features we want
    // don't need to do anything with it yet
    VkPhysicalDeviceFeatures features{};

    // create the device
    VkDeviceCreateInfo devCreate{};
    devCreate.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    devCreate.pQueueCreateInfos = &queueCreate;
    devCreate.queueCreateInfoCount = 1;
    devCreate.pEnabledFeatures = &features;
    // no extensions for now
    devCreate.enabledExtensionCount = 0;
    devCreate.ppEnabledExtensionNames = nullptr;

    std::vector<const char*> layerNames = {"VK_LAYER_KHRONOS_validation"};
    // add validation if we need to
    if (validationEnabled) {
      devCreate.enabledLayerCount = 1;
      devCreate.ppEnabledLayerNames = layerNames.data();
    } else {
      devCreate.enabledLayerCount = 0;
    }

    return vkCreateDevice(m_PhysicalDev, &devCreate, nullptr, &m_Device) == VK_SUCCESS;
  }


  bool Renderer::createSurface() {
    LinuxState* ls = (LinuxState*) Platform::s_State;
    // TODO: make this platform independent
    VkXcbSurfaceCreateInfoKHR create{};
    create.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    create.connection = ls->connection;
    create.window = ls->window;

    return vkCreateXcbSurfaceKHR(m_Instance, &create, nullptr, &m_Surface) == VK_SUCCESS;
  }
}

