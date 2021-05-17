#include "platform/platform.h"
#include "platform/linux/linux.h"
#include "octal/renderer/renderer.h"
#include "octal/renderer/shader.h"
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

    if (!createImageViews()) {
      FATAL("Failed to create imageviews");
      return false;
    }

    if (!createRenderPass()) {
      FATAL("Failed to create render pass!");
      return false;
    }

    if (!createGraphicsPipeline()) {
      FATAL("Failed to create graphics pipeline!");
      return false;
    }

    if (!createFramebuffers()) {
      FATAL("Failed to create framebuffers!");
      return false;
    }

    if (!createCommandPool()) {
      FATAL("Failed to create the commandpool!");
      return false;
    }

    if (!createCommandBuffers()) {
      FATAL("Failed to create the command buffers!");
      return false;
    }
    
    return true;
  }

  void Renderer::Shutdown() {
    // destroy the command pool
    vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
    // destroy framebuffers
    for (auto fb : m_SwapChainFramebuffers) {
      vkDestroyFramebuffer(m_Device, fb, nullptr);
    }
    // destroy the actual pipeline
    vkDestroyPipeline(m_Device, m_GraphicsPipeline, nullptr);
    // destroy the pipeline layout
    vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);
    // destroy the render pass
    vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);
    // destroy all the image views
    for (auto imageView : m_SwapChainImageViews) {
      vkDestroyImageView(m_Device, imageView, nullptr);
    }
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

    // set member variables too
    m_SwapChainFormat = format.format;
    m_SwapChainExtent = extent;

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

    // get the images

    if (vkCreateSwapchainKHR(m_Device, &create, nullptr, &m_SwapChain) != VK_SUCCESS) {
      return false;
    }

    vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, nullptr);
    m_SwapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, m_SwapChainImages.data());

    return true;
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

  bool Renderer::createImageViews() {
    // resize the image view vector to include however many images are in the swapchain
    m_SwapChainImageViews.resize(m_SwapChainImages.size());

    // create a new image view for each image
    for (int i = 0; i < m_SwapChainImages.size(); ++i) {
      VkImageViewCreateInfo create{};
      create.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      create.image = m_SwapChainImages[i];
      create.viewType = VK_IMAGE_VIEW_TYPE_2D;
      create.format = m_SwapChainFormat;

      // leave colors normal order
      create.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
      create.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
      create.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
      create.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

      // define purpose and region
      // here we just want to copy color
      create.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      // we only need one level since our sc has only one
      create.subresourceRange.baseMipLevel = 0;
      create.subresourceRange.levelCount = 1;
      create.subresourceRange.baseArrayLayer = 0;
      create.subresourceRange.layerCount = 1;

      if (vkCreateImageView(m_Device, &create, nullptr, &m_SwapChainImageViews[i]) != VK_SUCCESS) {
        ERROR("Could not create image %d", i);
        return false;
      }
    }

    return true;
  }

  bool Renderer::createRenderPass() {
    // describe our color only attachment
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = m_SwapChainFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // not doing multisampling
    // clear on load
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    // don't care about the stencil for now
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    // initial is undefined but we want to present to swapchain at the end
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // reference to the above attachment
    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // create a color subpass
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    return vkCreateRenderPass(m_Device, &renderPassInfo, nullptr, &m_RenderPass) == VK_SUCCESS;
  }

  bool Renderer::createGraphicsPipeline() {
    Shader vert = Shader(m_Device, "./assets/shaders/bin/vert.spv");
    Shader frag = Shader(m_Device, "./assets/shaders/bin/frag.spv");

    if (!vert.createShaderModule() || !frag.createShaderModule()) {
      ERROR("Could not create shader modules");
      return false;
    }

    VkPipelineShaderStageCreateInfo vertShaderStage{};
    vertShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStage.module = vert.module;
    vertShaderStage.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStage{};
    fragShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStage.module = frag.module;
    fragShaderStage.pName = "main";
    
    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStage, fragShaderStage};

    // vertex input (describes the layout of data in a vertex)
    // TODO: add vertex descriptors
    VkPipelineVertexInputStateCreateInfo vertexInput{};
    vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInput.vertexBindingDescriptionCount = 0;
    vertexInput.pVertexBindingDescriptions = nullptr;
    vertexInput.vertexAttributeDescriptionCount = 0;
    vertexInput.pVertexAttributeDescriptions = nullptr;

    // input assembly describes how to use all the vertices
    VkPipelineInputAssemblyStateCreateInfo inputAsm{};
    inputAsm.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAsm.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAsm.primitiveRestartEnable = VK_FALSE;

    // create the viewport
    VkViewport viewport{};
    viewport.x = 0.f;
    viewport.y = 0.f;
    viewport.width = (float) m_SwapChainExtent.width;
    viewport.height = (float) m_SwapChainExtent.height;
    viewport.minDepth = 0.f;
    viewport.minDepth = 1.f;

    // make the scissor the entire framebuffer since we want to show it all
    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = m_SwapChainExtent;

    // combine into viewport state!
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    // Rasterizing time!
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    // discard things that are too far away instead of clamping
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL; // TODO: look into using feature which allows lines
    rasterizer.lineWidth = 1.f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

    // don't fuck with the depth values
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.f;
    rasterizer.depthBiasSlopeFactor = 0.f;
    rasterizer.depthBiasClamp = 0.f;

    // Don't multisample for now
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;

    // TODO: add depth and stencil buffers here

    // Color blending (off for now)
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
    VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
    VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    //colorBlending.logicOpEnable = VK_TRUE;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    // we aren't using these so
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // OptionalVK_BLEND_FACTOR_ZERO

    // dynamic state, we'll keep this blank for now
    //VkDynamicState dynamicStates[] = { };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 0;
    dynamicState.pDynamicStates = nullptr;

    // pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayout{};
    pipelineLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayout.setLayoutCount = 0;
    pipelineLayout.pSetLayouts = nullptr;
    // can use these to pass dynamic values
    pipelineLayout.pushConstantRangeCount = 0;
    pipelineLayout.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(m_Device, &pipelineLayout, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
      ERROR("Could not create pipeline layout");
      return false;
    }

    // create the pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    // add shaders
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;

    pipelineInfo.pVertexInputState    = &vertexInput;
    pipelineInfo.pInputAssemblyState  = &inputAsm;
    pipelineInfo.pViewportState       = &viewportState;
    pipelineInfo.pRasterizationState  = &rasterizer;
    pipelineInfo.pMultisampleState    = &multisampling;
    pipelineInfo.pDepthStencilState   = nullptr;
    pipelineInfo.pColorBlendState     = &colorBlending;
    pipelineInfo.pDynamicState        = nullptr;

    pipelineInfo.layout = m_PipelineLayout;
    pipelineInfo.renderPass = m_RenderPass;
    pipelineInfo.subpass = 0;

    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    return vkCreateGraphicsPipelines(m_Device, VK_NULL_HANDLE, 
        1, &pipelineInfo, nullptr, &m_GraphicsPipeline) == VK_SUCCESS;
  }


  bool Renderer::createFramebuffers() {
    // resize the framebuffer vector to that of the image views
    m_SwapChainFramebuffers.resize(m_SwapChainImageViews.size());

    for (int i = 0; i < m_SwapChainFramebuffers.size(); ++i) {

      VkImageView attachments[] = { m_SwapChainImageViews[i] };

      VkFramebufferCreateInfo framebufferInfo{};
      framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      framebufferInfo.renderPass = m_RenderPass;
      framebufferInfo.attachmentCount = 1;
      framebufferInfo.pAttachments = attachments;
      framebufferInfo.width = m_SwapChainExtent.width;
      framebufferInfo.height = m_SwapChainExtent.height;
      framebufferInfo.layers = 1;

      if (vkCreateFramebuffer(m_Device, &framebufferInfo, nullptr, &m_SwapChainFramebuffers[i])
          != VK_SUCCESS) {
        ERROR("Could not create framebuffer #%d", i);
      }
    }

    return true;
  }

  bool Renderer::createCommandPool() {
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = m_QIndices.graphics.value();
    poolInfo.flags = 0;

    return vkCreateCommandPool(m_Device, &poolInfo, nullptr, &m_CommandPool) == VK_SUCCESS;
  }

  bool Renderer::createCommandBuffers() {
    // resize to the number of framebuffers we have
    m_CommandBuffers.resize(m_SwapChainFramebuffers.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_CommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (u32) m_CommandBuffers.size();

    if (vkAllocateCommandBuffers(m_Device, &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS) {
      ERROR("Failed to begin allocate command buffers");
      return false;
    }

    for (int i = 0; i < m_CommandBuffers.size(); ++i) {
      VkCommandBufferBeginInfo beginInfo{};
      beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      beginInfo.flags = 0;
      beginInfo.pInheritanceInfo = nullptr;
      if (vkBeginCommandBuffer(m_CommandBuffers[i], &beginInfo) != VK_SUCCESS) {
        ERROR("Failed to begin command buffer %d", i);
        return false;
      }
      
      VkRenderPassBeginInfo renderPassInfo{};
      renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderPassInfo.renderPass = m_RenderPass;
      renderPassInfo.framebuffer = m_SwapChainFramebuffers[i];
      renderPassInfo.renderArea.offset = {0,0};
      renderPassInfo.renderArea.extent = m_SwapChainExtent;

      VkClearValue clearColor = {0.f, 0.f, 0.f, 1.f};
      renderPassInfo.clearValueCount = 1;
      renderPassInfo.pClearValues = &clearColor;

      vkCmdBeginRenderPass(m_CommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(m_CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);
        vkCmdDraw(m_CommandBuffers[i], 3, 1, 0, 0);

      vkCmdEndRenderPass(m_CommandBuffers[i]);

      if (vkEndCommandBuffer(m_CommandBuffers[i]) != VK_SUCCESS) {
        ERROR("failed to record command buffer %d", i);
        return false;
      }
    }
    return true;
  }
}

