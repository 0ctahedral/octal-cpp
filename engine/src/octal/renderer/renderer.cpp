#include "platform/platform.h"
#include "platform/linux/linux.h"
#include "octal/renderer/renderer.h"
#include "octal/core/logger.h"
#include <cstring>
#include <set>
#include <string>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_xcb.h>

namespace octal {
  bool Renderer::Init() {
    if (!createInstance()) {
      FATAL("Failed to create vk instance");
      return false;
    }

    if (!setupDebugMesenger()) {
      FATAL("Failed to setup debugger");
      return false;
    }

    if (!createSurface()) {
      FATAL("Failed to create surface");
      return false;
    }

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
    vkGetDeviceQueue(m_Device, m_QIndices.present.value(), 0, &m_PresentQ);

    if (!createSwapChain()) {
      FATAL("Failed to create swapchain");
      return false;
    }

    return true;
  }

  void Renderer::Shutdown() {
    // destroy the swapchain
    vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);
    // Destroy the surface
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


    // TODO: make set of required extensions not hardcoded

    std::set<std::string> requiredExt(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

    // Check suitability of devices
    for (auto& dev : devices) {
      VkPhysicalDeviceProperties PDprops;
      vkGetPhysicalDeviceProperties(dev, &PDprops);
      VkPhysicalDeviceFeatures features;
      vkGetPhysicalDeviceFeatures(dev, &features);

      // print out the devices cuz why not
      INFO("Device %s: type=%d", PDprops.deviceName, PDprops.deviceType);

      m_QIndices = findQueueFamilies(dev);
      // TODO: select best device if we have more than one
      // ex: prefer descrete over integrated graphics
      if (features.geometryShader && m_QIndices.isComplete()) {
        *pd = dev;
        return true;
      }

      // check if the desired extensions are supported
      u32 extCount;
      vkEnumerateDeviceExtensionProperties(dev, nullptr, &extCount, nullptr);
      std::vector<VkExtensionProperties> availExt(extCount);
      vkEnumerateDeviceExtensionProperties(dev, nullptr, &extCount, availExt.data());


      for (const auto& ext : availExt) {
        requiredExt.erase(std::string(ext.extensionName));
      }
    }

    return requiredExt.empty(); 
  }


  QueueFamilyIndices Renderer::findQueueFamilies(VkPhysicalDevice dev) {
    QueueFamilyIndices indices;

    u32 queueCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(dev, &queueCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueCount);

    vkGetPhysicalDeviceQueueFamilyProperties(dev, &queueCount, queueFamilies.data());

    for (u32 i = 0; i < queueCount; ++i) {
      // check for graphics support
      if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        indices.graphics = i;
      }
      // check for presentation support
      VkBool32 presentSupport = false;
      vkGetPhysicalDeviceSurfaceSupportKHR(dev, i, m_Surface, &presentSupport);
      if (presentSupport) {
        indices.present = i;
      }
    }

    return indices;
  }

  bool Renderer::createLogicalDevice() {
    // creates the device queues
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};
    // create a set of queue indices. by using a set we insure that we don't repeat an index
    std::set<u32> uniqueQueueFam = {m_QIndices.graphics.value(), m_QIndices.present.value()};
    float priority = 1.f;
    for (u32 qf : uniqueQueueFam) {
      VkDeviceQueueCreateInfo queueCreate{};
      queueCreate.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      // set graphics index
      queueCreate.queueFamilyIndex = qf;
      queueCreate.queueCount = 1;
      // make this the highest priority
      queueCreate.pQueuePriorities = &priority;
      // add it to our creates
      queueCreateInfos.push_back(queueCreate);
    }

    // device features we want
    // don't need to do anything with it yet
    VkPhysicalDeviceFeatures features{};

    // create the device
    VkDeviceCreateInfo devCreate{};
    devCreate.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    devCreate.pQueueCreateInfos = queueCreateInfos.data();
    devCreate.queueCreateInfoCount = queueCreateInfos.size();
    devCreate.pEnabledFeatures = &features;
    // no extensions for now
    devCreate.enabledExtensionCount = m_DeviceExtensions.size();
    devCreate.ppEnabledExtensionNames = m_DeviceExtensions.data();

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

  bool Renderer::createSwapChain() {
    // tell us about the surface and device capabilities
    SwapchainDetails details = querySwapchainSupport(m_PhysicalDev, m_Surface);
    if (details.formats.size() == 0 || details.modes.size() == 0) {
      ERROR("Not enough formats or modes:\nformats: %d, modes: %d", details.formats.size(), details.modes.size());
      return false;
    }

    // get the best format
    auto format = chooseFormat(details.formats);
    // get the best mode
    auto mode = chooseMode(details.modes);
    // get extents setup
    auto extent = chooseExtent(details.capabilities);

    // create swapchain
    // always have at least one more image than the minimum
    u32 imageCount = details.capabilities.minImageCount + 1;

    // set the image count to max iff we exceed it
    if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount) {
      imageCount = details.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create{};
    create.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create.surface = m_Surface;
    create.minImageCount = imageCount;
    create.imageFormat = format.format;
    create.imageColorSpace = format.colorSpace;
    create.imageExtent = extent;
    create.imageArrayLayers = 1;
    create.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    u32 indices[] = {m_QIndices.graphics.value(), m_QIndices.present.value()};

    // if the graphics and present queues are different
    if (m_QIndices.graphics != m_QIndices.present) {
      create.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
      create.queueFamilyIndexCount = 2;
      create.pQueueFamilyIndices = indices;
    } else {
      create.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
      create.queueFamilyIndexCount = 0;
      create.pQueueFamilyIndices = nullptr;
    }

    // keep transform the same
    create.preTransform = details.capabilities.currentTransform;
    // ignore alpha rn
    create.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    create.presentMode = mode;
    // we don't care about pixels that are covered by another window
    create.clipped = VK_TRUE;
    create.oldSwapchain = VK_NULL_HANDLE;

    return vkCreateSwapchainKHR(m_Device, &create, nullptr, &m_SwapChain) == VK_SUCCESS;
  }

  SwapchainDetails Renderer::querySwapchainSupport(VkPhysicalDevice dev, VkSurfaceKHR surface) {
    SwapchainDetails ret;
    // get capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(dev, surface, &ret.capabilities);

    // get formats
    u32 formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(dev, surface, &formatCount, nullptr);
    ret.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(dev, surface, &formatCount, ret.formats.data());

    // get modes
    u32 modeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(dev, surface, &modeCount, nullptr);
    ret.modes.resize(modeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(dev, surface, &modeCount, ret.modes.data());

    return ret;
  }

  VkPresentModeKHR Renderer::chooseMode(const std::vector<VkPresentModeKHR>& modes) {
    for (const auto& m : modes) {
      if (m == VK_PRESENT_MODE_MAILBOX_KHR)
        return m;
    }

    // fallback to the only one that is guranteed
    return VK_PRESENT_MODE_FIFO_KHR;
  }

  VkSurfaceFormatKHR Renderer::chooseFormat(const std::vector<VkSurfaceFormatKHR>& formats) {
    // format:
    for (const auto& f: formats) {
      if (f.format == VK_FORMAT_B8G8R8A8_SRGB && f.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
         return f;
    }

    // return first one if we cannot find ideal
    return formats[0];
  }

  VkExtent2D Renderer::chooseExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
    // if it has a normal extent then just do it
    if (capabilities.currentExtent.width != UINT32_MAX)
      return capabilities.currentExtent;

    VkExtent2D actual = {1920, 1080};
    // get the best extent we can
    actual.width = std::max(capabilities.minImageExtent.width,
        std::min(capabilities.maxImageExtent.width, actual.width));
    actual.height = std::max(capabilities.minImageExtent.height,
        std::min(capabilities.maxImageExtent.height, actual.height));

    return actual;
  }
}

