/***************************************************************************
 *   Copyright (C) 2009 by Erik Sohns   *
 *   erik.sohns@web.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "stdafx.h"

#include "test_i_app.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <unordered_map>

#include "vulkan/vk_enum_string_helper.h"

#if defined (GLM_SUPPORT)
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"
#endif // GLM_SUPPORT

#if defined (TINY_OBJ_LOADER_SUPPORT)
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#endif // TINY_OBJ_LOADER_SUPPORT

#if defined (STB_IMAGE_SUPPORT)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif // STB_IMAGE_SUPPORT

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined (ACE_LINUX)
// #define GLFW_EXPOSE_NATIVE_WAYLAND
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_NONE
#endif // ACE_WIN32 || ACE_WIN64 || ACE_LINUX
#include "GLFW/glfw3native.h"

#include "ace/Log_Msg.h"
#include "ace/OS_NS_string.h"

#include "common_file_tools.h"

#include "test_i_vulkan_common.h"

namespace std
{
template <> struct hash<Vertex>
{
  size_t operator() (Vertex const& vertex) const
  {
    return ((hash<glm::vec3> () (vertex.pos) ^ (hash<glm::vec3> () (vertex.color) << 1)) >> 1) ^ (hash<glm::vec2> () (vertex.texCoord) << 1);
  }
};
}

const std::vector<const char*> validation_layers_a = {
  ACE_TEXT_ALWAYS_CHAR ("VK_LAYER_KHRONOS_validation")
};
const std::vector<const char*> device_extensions_a = {
  VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

//const std::vector<Vertex> vertices = { {  {0.0f, -0.5f}, {1.0f, 0.0f, 0.0f} }, // triangle
//                                       {  {0.5f,  0.5f}, {0.0f, 1.0f, 0.0f} },
//                                       { {-0.5f,  0.5f}, {0.0f, 0.0f, 1.0f} } };
// const std::vector<Vertex> vertices_2 = { { {-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f} }, // quad
//                                          { { 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f} },
//                                          { { 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f} },
//                                          { {-0.5f,  0.5f, 0.0f}, {1.0f, 1.0f, 1.0f} } };
// const std::vector<Vertex> vertices_3 = { { {-0.5f, -0.5f,  0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} },
//                                          { { 0.5f, -0.5f,  0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f} },
//                                          { { 0.5f,  0.5f,  0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f} },
//                                          { {-0.5f,  0.5f,  0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f} },
//                                          { {-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} },
//                                          { { 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f} },
//                                          { { 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f} },
//                                          { {-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f} } };
// const std::vector<uint32_t> indices = { 0, 1, 2, 2, 3, 0,   // quad
//                                         4, 5, 6, 6, 7, 4 }; // quad

struct UniformBufferObject
{
  alignas (16) glm::mat4 model;
  alignas (16) glm::mat4 view;
  alignas (16) glm::mat4 proj;
};

/////////////////////////////////////////

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback (VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                     VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                     const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                     void* pUserData)
{
  std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

  return VK_FALSE;
}

/////////////////////////////////////////

HelloTriangleApplication::HelloTriangleApplication ()
 : framebufferResized_ (false)
 , commandBuffers_ ()
 , commandPool_ (VK_NULL_HANDLE)
 , debugMessenger_ (VK_NULL_HANDLE)
 , device_ (VK_NULL_HANDLE)
 , graphicsPipeline_ (VK_NULL_HANDLE)
 , graphicsQueue_ (VK_NULL_HANDLE)
 , instance_ (VK_NULL_HANDLE)
 , physicalDevice_ (VK_NULL_HANDLE)
 , pipelineLayout_ (VK_NULL_HANDLE)
 , presentQueue_ (VK_NULL_HANDLE)
 , renderPass_ (VK_NULL_HANDLE)
 , surface_ (VK_NULL_HANDLE)
 , swapChain_ (VK_NULL_HANDLE)
 , swapChainExtent_ ()
 , swapChainFramebuffers_ ()
 , swapChainImageFormat_ (VK_FORMAT_UNDEFINED)
 , swapChainImages_ ()
 , swapChainImageViews_ ()
 , imageAvailableSemaphores_ ()
 , renderFinishedSemaphores_ ()
 , inFlightFences_ ()
 , vertexBuffer_ (VK_NULL_HANDLE)
 , vertexBufferMemory_ (VK_NULL_HANDLE)
 , indexBuffer_ (VK_NULL_HANDLE)
 , indexBufferMemory_ (VK_NULL_HANDLE)
 , uniformBuffers_ ()
 , uniformBuffersMemory_ ()
 , uniformBuffersMapped_ ()
 , descriptorSetLayout_ (VK_NULL_HANDLE)
 , descriptorPool_ (VK_NULL_HANDLE)
 , descriptorSets_ ()
 , textureImage_ (VK_NULL_HANDLE)
 , textureImageMemory_ (VK_NULL_HANDLE)
 , textureImageView_ (VK_NULL_HANDLE)
 , textureSampler_ (VK_NULL_HANDLE)
 , depthImage_ (VK_NULL_HANDLE)
 , depthImageMemory_ (VK_NULL_HANDLE)
 , depthImageView_ (VK_NULL_HANDLE)
 , vertices_ ()
 , indices_ ()
 , currentFrame_ (0)
 , window_ (NULL)
{
  glfwWindowHint (GLFW_CLIENT_API, GLFW_NO_API); // do not create an OpenGL context
  // glfwWindowHint (GLFW_RESIZABLE, GLFW_FALSE);
}

static void framebufferResizeCallback (GLFWwindow* window,
                                       int width,
                                       int height)
{
  HelloTriangleApplication* app =
    reinterpret_cast<HelloTriangleApplication*> (glfwGetWindowUserPointer (window));
  app->framebufferResized_ = true;
}

void
HelloTriangleApplication::initWindow ()
{ ACE_ASSERT (!window_);
  window_ = glfwCreateWindow (TEST_I_VULKAN_DEFAULT_WINDOW_WIDTH,
                              TEST_I_VULKAN_DEFAULT_WINDOW_HEIGHT,
                              ACE_TEXT_ALWAYS_CHAR ("Vulkan window"),
                              NULL, NULL);
  ACE_ASSERT (window_);

  glfwSetWindowUserPointer (window_, this);
  glfwSetFramebufferSizeCallback (window_, framebufferResizeCallback);
}

bool
HelloTriangleApplication::checkValidationLayerSupport (const std::vector<const char*>& validationLayers_in)
{
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties (&layerCount, NULL);
  std::vector<VkLayerProperties> availableLayers (layerCount);
  vkEnumerateInstanceLayerProperties (&layerCount, availableLayers.data ());

  for (const char* layerName: validationLayers_in)
  {
    bool layerFound = false;
    for (const auto& layerProperties: availableLayers)
    {
      if (ACE_OS::strcmp (layerName, layerProperties.layerName) == 0)
      {
        layerFound = true;
        break;
      } // end IF
    } // end FOR
    if (!layerFound)
      return false;
  } // end FOR

  return true;
}

std::vector<const char*>
HelloTriangleApplication::getRequiredExtensions (bool enableValidationLayers_in)
{
  uint32_t glfwExtensionCount = 0;
  const char** glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions (&glfwExtensionCount);

  std::vector<const char*> extensions (glfwExtensions, glfwExtensions + glfwExtensionCount);
  if (enableValidationLayers_in)
    extensions.push_back (VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

#if defined (ACE_LINUX)
  // extensions.push_back (ACE_TEXT_ALWAYS_CHAR ("VK_KHR_wayland_surface"));
  extensions.push_back (ACE_TEXT_ALWAYS_CHAR ("VK_KHR_xlib_surface"));
#endif // ACE_LINUX

  return extensions;
}

void
HelloTriangleApplication::createInstance (bool enableValidationLayers_in)
{
  VkApplicationInfo appInfo {};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = ACE_TEXT_ALWAYS_CHAR ("Hello Triangle");
  appInfo.applicationVersion = VK_MAKE_VERSION (1,0,0);
  appInfo.pEngineName = ACE_TEXT_ALWAYS_CHAR ("No Engine");
  appInfo.engineVersion = VK_MAKE_VERSION (1,0,0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo createInfo {};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;
  std::vector<const char*> required_extensions_a =
    getRequiredExtensions (enableValidationLayers_in);
  createInfo.enabledExtensionCount = static_cast<uint32_t> (required_extensions_a.size ());
  createInfo.ppEnabledExtensionNames = required_extensions_a.data ();

  // if (enableValidationLayers_in && !checkValidationLayerSupport (validation_layers_a))
  //   return;
  // VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo {};
  // if (enableValidationLayers_in)
  // {
  //   createInfo.enabledLayerCount =
  //     static_cast<uint32_t> (validation_layers_a.size ());
  //   createInfo.ppEnabledLayerNames = validation_layers_a.data ();

  //   populateDebugMessengerCreateInfo (debugCreateInfo);
  //   createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
  // } // end IF
  // else
  {
    createInfo.enabledLayerCount = 0;
    createInfo.pNext = NULL;
  } // end ELSE

  VkResult result = vkCreateInstance (&createInfo, NULL, &instance_);
  if (result != VK_SUCCESS)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to vkCreateInstance(): \"%s\"\n"),
                ACE_TEXT (string_VkResult (result))));
}

VkResult
CreateDebugUtilsMessengerEXT (VkInstance instance,
                              const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                              const VkAllocationCallbacks* pAllocator,
                              VkDebugUtilsMessengerEXT* pDebugMessenger)
{
  auto func =
    (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr (instance, ACE_TEXT_ALWAYS_CHAR ("vkCreateDebugUtilsMessengerEXT"));
  if (func != NULL) {
    return func (instance, pCreateInfo, pAllocator, pDebugMessenger);
  }
  return VK_ERROR_EXTENSION_NOT_PRESENT;
}
void
DestroyDebugUtilsMessengerEXT (VkInstance instance,
                               VkDebugUtilsMessengerEXT debugMessenger,
                               const VkAllocationCallbacks* pAllocator)
{
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr (instance, ACE_TEXT_ALWAYS_CHAR ("vkDestroyDebugUtilsMessengerEXT"));
  if (func != NULL) {
    func (instance, debugMessenger, pAllocator);
  }
}

void
HelloTriangleApplication::populateDebugMessengerCreateInfo (VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
  createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT    |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = debugCallback;
}

void
HelloTriangleApplication::setupDebugMessenger (bool enableValidationLayers_in)
{
  if (!enableValidationLayers_in)
    return;

  VkDebugUtilsMessengerCreateInfoEXT createInfo {};
  populateDebugMessengerCreateInfo (createInfo);
  createInfo.pUserData = NULL; // Optional

  VkResult result = CreateDebugUtilsMessengerEXT (instance_,
                                                  &createInfo,
                                                  NULL,
                                                  &debugMessenger_);
  if (result != VK_SUCCESS)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to setup debug messenger: \"%s\"\n"),
                ACE_TEXT (string_VkResult (result))));
}

void
HelloTriangleApplication::initVulkan (bool enableValidationLayers_in)
{
  //uint32_t extensionCount = 0;
  //vkEnumerateInstanceExtensionProperties (NULL, &extensionCount, NULL);
  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("%u extensions supported\n"),
  //            extensionCount));

  createInstance (enableValidationLayers_in);
  setupDebugMessenger (enableValidationLayers_in);
  createSurface ();
  pickPhysicalDevice ();
  createLogicalDevice (enableValidationLayers_in);
  createSwapChain ();
  createImageViews ();
  createRenderPass ();
  createDescriptorSetLayout ();
  createGraphicsPipeline ();
  createCommandPool ();
  createDepthResources ();
  createFramebuffers ();
  createTextureImage ();
  createTextureImageView ();
  createTextureSampler ();
  loadModel ();
  createVertexBuffer ();
  createIndexBuffer ();
  createUniformBuffers ();
  createDescriptorPool ();
  createDescriptorSets ();
  createCommandBuffers ();
  createSyncObjects ();
}

HelloTriangleApplication::QueueFamilyIndices
HelloTriangleApplication::findQueueFamilies (VkPhysicalDevice device)
{
  QueueFamilyIndices indices;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties (device, &queueFamilyCount, NULL);

  std::vector<VkQueueFamilyProperties> queue_families_a (queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties (device, &queueFamilyCount, queue_families_a.data ());

  int i = 0;
  VkBool32 presentSupport;
  for (const auto& queueFamily: queue_families_a)
  {
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
    {
      indices.graphicsFamily = i;
    } // end IF

    presentSupport = 0;
    vkGetPhysicalDeviceSurfaceSupportKHR (device, i, surface_, &presentSupport);
    if (presentSupport)
    {
      indices.presentFamily = i;
    } // end IF

    if (indices.isComplete ())
      break;

    i++;
  } // end FOR

  return indices;
}

bool
HelloTriangleApplication::checkDeviceExtensionSupport (VkPhysicalDevice device)
{
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties (device, NULL, &extensionCount, NULL);

  std::vector<VkExtensionProperties> availableExtensions (extensionCount);
  vkEnumerateDeviceExtensionProperties (device, NULL, &extensionCount, availableExtensions.data ());

  std::set<std::string> requiredExtensions (device_extensions_a.begin (),
                                            device_extensions_a.end ());
  for (const auto& extension: availableExtensions)
    requiredExtensions.erase (extension.extensionName);

  return requiredExtensions.empty ();
}

bool
HelloTriangleApplication::isDeviceSuitable (VkPhysicalDevice device)
{
  //VkPhysicalDeviceProperties deviceProperties;
  //VkPhysicalDeviceFeatures deviceFeatures;
  //vkGetPhysicalDeviceProperties (device, &deviceProperties);
  //vkGetPhysicalDeviceFeatures (device, &deviceFeatures);

  QueueFamilyIndices indices = findQueueFamilies (device);

  bool extensionsSupported = checkDeviceExtensionSupport (device);

  bool swapChainAdequate = false;
  if (extensionsSupported)
  {
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport (device);
    swapChainAdequate = !swapChainSupport.formats.empty () && !swapChainSupport.presentModes.empty ();
  } // end IF

  VkPhysicalDeviceFeatures supportedFeatures;
  vkGetPhysicalDeviceFeatures (device, &supportedFeatures);

  return indices.isComplete () && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;

  //return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
  //       deviceFeatures.geometryShader;
}

void
HelloTriangleApplication::pickPhysicalDevice ()
{
  uint32_t deviceCount;
  vkEnumeratePhysicalDevices (instance_, &deviceCount, NULL);
  if (deviceCount == 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to find GPUs with Vulkan support\n")));
    return;
  } // end IF
  std::vector<VkPhysicalDevice> devices_a (deviceCount);
  vkEnumeratePhysicalDevices (instance_, &deviceCount, devices_a.data ());
  for (const auto& device: devices_a)
    if (isDeviceSuitable (device))
    {
      physicalDevice_ = device;
      break;
    } // end IF

  if (physicalDevice_ == VK_NULL_HANDLE)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to find a suitable GPU\n")));
    return;
  } // end IF
}

HelloTriangleApplication::SwapChainSupportDetails
HelloTriangleApplication::querySwapChainSupport (VkPhysicalDevice device)
{
  SwapChainSupportDetails details;

  VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR (device, surface_, &details.capabilities);
  if (result != VK_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to vkGetPhysicalDeviceSurfaceCapabilitiesKHR(): \"%s\"\n"),
                ACE_TEXT (string_VkResult (result))));
    return {};
  } // end IF

  uint32_t formatCount;
  result = vkGetPhysicalDeviceSurfaceFormatsKHR (device, surface_, &formatCount, NULL);
  ACE_ASSERT (result == VK_SUCCESS);
  if (formatCount != 0)
  {
    details.formats.resize (formatCount);
    result = vkGetPhysicalDeviceSurfaceFormatsKHR (device, surface_, &formatCount, details.formats.data ());
    if (result != VK_SUCCESS)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to vkGetPhysicalDeviceSurfaceFormatsKHR(): \"%s\"\n"),
                  ACE_TEXT (string_VkResult (result))));
      return {};
    } // end IF
  } // end IF

  uint32_t presentModeCount;
  result = vkGetPhysicalDeviceSurfacePresentModesKHR (device, surface_, &presentModeCount, NULL);
  ACE_ASSERT (result == VK_SUCCESS);
  if (presentModeCount != 0)
  {
    details.presentModes.resize (presentModeCount);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR (device, surface_, &presentModeCount, details.presentModes.data ());
    if (result != VK_SUCCESS)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to vkGetPhysicalDeviceSurfacePresentModesKHR(): \"%s\"\n"),
                  ACE_TEXT (string_VkResult (result))));
      return {};
    } // end IF
  } // end IF

  return details;
}

void 
HelloTriangleApplication::createLogicalDevice (bool enableValidationLayers_in)
{
  QueueFamilyIndices indices = findQueueFamilies (physicalDevice_);

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value (),
                                             indices.presentFamily.value () };

  float queuePriority = 1.0f;
  for (uint32_t queueFamily: uniqueQueueFamilies)
  {
    VkDeviceQueueCreateInfo queueCreateInfo {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back (queueCreateInfo);
  } // end FOR

  VkPhysicalDeviceFeatures deviceFeatures {};
  deviceFeatures.samplerAnisotropy = VK_TRUE;

  VkDeviceCreateInfo createInfo {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.queueCreateInfoCount = static_cast<uint32_t> (queueCreateInfos.size ());
  createInfo.pQueueCreateInfos = queueCreateInfos.data ();
  createInfo.pEnabledFeatures = &deviceFeatures;

  createInfo.enabledExtensionCount = static_cast<uint32_t> (device_extensions_a.size ());
  createInfo.ppEnabledExtensionNames = device_extensions_a.data ();

  if (enableValidationLayers_in)
  {
    createInfo.enabledLayerCount =
      static_cast<uint32_t> (validation_layers_a.size ());
    createInfo.ppEnabledLayerNames = validation_layers_a.data ();
  } // end IF
  else
    createInfo.enabledLayerCount = 0;

  VkResult result = vkCreateDevice (physicalDevice_,
                                    &createInfo,
                                    NULL,
                                    &device_);
  if (result != VK_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to create logical device: \"%s\"\n"),
                ACE_TEXT (string_VkResult (result))));
    return;
  } // end IF
  vkGetDeviceQueue (device_,
                    indices.graphicsFamily.value (),
                    0,
                    &graphicsQueue_);
  vkGetDeviceQueue (device_,
                    indices.presentFamily.value (),
                    0,
                    &presentQueue_);
}

void
HelloTriangleApplication::createSurface ()
{
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  VkWin32SurfaceCreateInfoKHR createInfo {};
  createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
  createInfo.hwnd = glfwGetWin32Window (window_);
  createInfo.hinstance = GetModuleHandle (NULL);

  VkResult result = vkCreateWin32SurfaceKHR (instance_,
                                             &createInfo,
                                             NULL,
                                             &surface_);
  if (result != VK_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to create window surface: \"%s\"\n"),
                ACE_TEXT (string_VkResult (result))));
    return;
  } // end IF
#elif defined (ACE_LINUX)
  // VkWaylandSurfaceCreateInfoKHR createInfo {};
  // createInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
  // createInfo.display = glfwGetWaylandDisplay ();
  // createInfo.surface = glfwGetWaylandWindow (window_);

  // VkResult result = vkCreateWaylandSurfaceKHR (instance_,
  //                                              &createInfo,
  //                                              NULL,
  //                                              &surface_);
  VkXlibSurfaceCreateInfoKHR createInfo {};
  createInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
  createInfo.dpy = glfwGetX11Display ();
  createInfo.window = glfwGetX11Window (window_);

  VkResult result = vkCreateXlibSurfaceKHR (instance_,
                                            &createInfo,
                                            NULL,
                                            &surface_);
  if (result != VK_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to create window surface: \"%s\"\n"),
                ACE_TEXT (string_VkResult (result))));
    return;
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64 || ACE_LINUX
}

VkSurfaceFormatKHR
HelloTriangleApplication::chooseSwapSurfaceFormat (const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
  for (const auto& availableFormat: availableFormats)
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
        availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
      return availableFormat;

  return availableFormats[0];
}
VkPresentModeKHR
HelloTriangleApplication::chooseSwapPresentMode (const std::vector<VkPresentModeKHR>& availablePresentModes)
{
  for (const auto& availablePresentMode: availablePresentModes)
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
      return availablePresentMode;

  return VK_PRESENT_MODE_FIFO_KHR;
}
VkExtent2D
HelloTriangleApplication::chooseSwapExtent (const VkSurfaceCapabilitiesKHR& capabilities)
{
  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max ())
    return capabilities.currentExtent;

  int width, height;
  glfwGetFramebufferSize (window_, &width, &height);

  VkExtent2D actualExtent = {static_cast<uint32_t> (width),
                             static_cast<uint32_t> (height)};
  actualExtent.width =
    std::clamp (actualExtent.width,
                capabilities.minImageExtent.width,
                capabilities.maxImageExtent.width);
  actualExtent.height =
    std::clamp (actualExtent.height,
                capabilities.minImageExtent.height,
                capabilities.maxImageExtent.height);

  return actualExtent;
}

void
HelloTriangleApplication::createSwapChain ()
{
  SwapChainSupportDetails swapChainSupport = querySwapChainSupport (physicalDevice_);

  VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat (swapChainSupport.formats);
  swapChainImageFormat_ = surfaceFormat.format;
  VkPresentModeKHR presentMode = chooseSwapPresentMode (swapChainSupport.presentModes);
  swapChainExtent_ = chooseSwapExtent (swapChainSupport.capabilities);

  uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
  if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
    imageCount = swapChainSupport.capabilities.maxImageCount;

  VkSwapchainCreateInfoKHR createInfo {};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = surface_;
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = swapChainExtent_;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  QueueFamilyIndices indices = findQueueFamilies (physicalDevice_);
  uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value (),
                                    indices.presentFamily.value ()};
  if (indices.graphicsFamily != indices.presentFamily)
  {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  } // end IF
  else
  {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0; // Optional
    createInfo.pQueueFamilyIndices = NULL; // Optional
  } // end ELSE

  createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;
  createInfo.oldSwapchain = VK_NULL_HANDLE;

  VkResult result = vkCreateSwapchainKHR (device_,
                                          &createInfo,
                                          NULL,
                                          &swapChain_);
  if (result != VK_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to create swap chain: \"%s\"\n"),
                ACE_TEXT (string_VkResult (result))));
    return;
  } // end IF

  result = vkGetSwapchainImagesKHR (device_, swapChain_, &imageCount, NULL);
  ACE_ASSERT (result == VK_SUCCESS);
  swapChainImages_.resize (imageCount);
  result = vkGetSwapchainImagesKHR (device_, swapChain_, &imageCount, swapChainImages_.data ());
  ACE_ASSERT (result == VK_SUCCESS);
}

void
HelloTriangleApplication::createImageViews ()
{
  swapChainImageViews_.resize (swapChainImages_.size ());

  for (size_t i = 0; i < swapChainImages_.size (); i++)
    swapChainImageViews_[i] =
      createImageView (swapChainImages_[i], swapChainImageFormat_, VK_IMAGE_ASPECT_COLOR_BIT);
}

static
std::vector<char>
readFile (const std::string& filename)
{
  std::vector<char> buffer;

  std::ifstream file (filename, std::ios::ate | std::ios::binary);

  if (!file.is_open ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to open file: \"%s\", aborting\n"),
                ACE_TEXT (filename.c_str ())));
    return buffer;
  } // end IF

  size_t fileSize = (size_t)file.tellg ();
  buffer.resize (fileSize);
  file.seekg (0);
  file.read (buffer.data (), fileSize);

  file.close ();

  return buffer;
}

VkShaderModule
HelloTriangleApplication::createShaderModule (const std::vector<char>& code)
{
  VkShaderModuleCreateInfo createInfo {};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size ();
  createInfo.pCode = reinterpret_cast<const uint32_t*> (code.data ());

  VkShaderModule shaderModule;
  VkResult result = vkCreateShaderModule (device_,
                                          &createInfo,
                                          NULL,
                                          &shaderModule);
  if (result != VK_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to create shader module: \"%s\", aborting\n"),
                ACE_TEXT (string_VkResult (result))));
    return shaderModule;
  } // end IF

  return shaderModule;
}

void
HelloTriangleApplication::createRenderPass ()
{
  VkAttachmentDescription colorAttachment {};
  colorAttachment.format = swapChainImageFormat_;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef {};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription depthAttachment {};
  depthAttachment.format = findDepthFormat ();
  depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout =
    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthAttachmentRef {};
  depthAttachmentRef.attachment = 1;
  depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;
  subpass.pDepthStencilAttachment = &depthAttachmentRef;

  VkSubpassDependency dependency {};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                             VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

  std::array<VkAttachmentDescription, 2> attachments = { colorAttachment,
                                                         depthAttachment };
  VkRenderPassCreateInfo renderPassInfo {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = static_cast<uint32_t> (attachments.size ());
  renderPassInfo.pAttachments = attachments.data ();
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  VkResult result = vkCreateRenderPass (device_,
                                        &renderPassInfo,
                                        NULL,
                                        &renderPass_);
  if (result != VK_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to create render pass: \"%s\", returning\n"),
                ACE_TEXT (string_VkResult (result))));
    return;
  } // end IF
}

void
HelloTriangleApplication::createDescriptorSetLayout ()
{
  VkDescriptorSetLayoutBinding uboLayoutBinding {};
  uboLayoutBinding.binding = 0;
  uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding.descriptorCount = 1;
  uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  uboLayoutBinding.pImmutableSamplers = NULL; // Optional

  VkDescriptorSetLayoutBinding samplerLayoutBinding {};
  samplerLayoutBinding.binding = 1;
  samplerLayoutBinding.descriptorType =
    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerLayoutBinding.descriptorCount = 1;
  samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  samplerLayoutBinding.pImmutableSamplers = NULL;

  std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding,
                                                          samplerLayoutBinding};

  VkDescriptorSetLayoutCreateInfo layoutInfo {};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t> (bindings.size ());
  layoutInfo.pBindings = bindings.data ();

  VkResult result = vkCreateDescriptorSetLayout (device_,
                                                 &layoutInfo,
                                                 NULL,
                                                 &descriptorSetLayout_);
  if (result != VK_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to create descriptor set layout: \"%s\", returning\n"),
                ACE_TEXT (string_VkResult (result))));
    return;
  } // end IF
}

void
HelloTriangleApplication::createGraphicsPipeline ()
{
  std::vector<char> vertShaderCode = readFile ("etc/vert.spv");
  std::vector<char> fragShaderCode = readFile ("etc/frag.spv");

  VkShaderModule vertShaderModule = createShaderModule (vertShaderCode);
  VkShaderModule fragShaderModule = createShaderModule (fragShaderCode);

  VkPipelineShaderStageCreateInfo vertShaderStageInfo {};
  vertShaderStageInfo.sType =
    VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = vertShaderModule;
  vertShaderStageInfo.pName = ACE_TEXT_ALWAYS_CHAR ("main");

  VkPipelineShaderStageCreateInfo fragShaderStageInfo {};
  fragShaderStageInfo.sType =
    VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = fragShaderModule;
  fragShaderStageInfo.pName = ACE_TEXT_ALWAYS_CHAR ("main");

  VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo,
                                                     fragShaderStageInfo };

  std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT,
                                                VK_DYNAMIC_STATE_SCISSOR };
  VkPipelineDynamicStateCreateInfo dynamicState {};
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.dynamicStateCount = static_cast<uint32_t> (dynamicStates.size ());
  dynamicState.pDynamicStates = dynamicStates.data ();

  VkPipelineVertexInputStateCreateInfo vertexInputInfo {};
  vertexInputInfo.sType =
    VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  VkVertexInputBindingDescription bindingDescription = Vertex::getBindingDescription ();
  vertexInputInfo.vertexBindingDescriptionCount = 1;
  vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
  std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions =
    Vertex::getAttributeDescriptions ();
  vertexInputInfo.vertexAttributeDescriptionCount =
    static_cast<uint32_t> (attributeDescriptions.size ());
  vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data ();

  VkPipelineInputAssemblyStateCreateInfo inputAssembly {};
  inputAssembly.sType =
    VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  VkViewport viewport {};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)swapChainExtent_.width;
  viewport.height = (float)swapChainExtent_.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor {};
  scissor.offset = {0, 0};
  scissor.extent = swapChainExtent_;

  VkPipelineViewportStateCreateInfo viewportState {};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.pViewports = &viewport;
  viewportState.scissorCount = 1;
  viewportState.pScissors = &scissor;

  VkPipelineRasterizationStateCreateInfo rasterizer {};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  //rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;
  rasterizer.depthBiasConstantFactor = 0.0f; // Optional
  rasterizer.depthBiasClamp = 0.0f;          // Optional
  rasterizer.depthBiasSlopeFactor = 0.0f;    // Optional

  VkPipelineMultisampleStateCreateInfo multisampling {};
  multisampling.sType =
    VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling.minSampleShading = 1.0f;          // Optional
  multisampling.pSampleMask = NULL;               // Optional
  multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
  multisampling.alphaToOneEnable = VK_FALSE;      // Optional

  VkPipelineColorBlendAttachmentState colorBlendAttachment {};
  colorBlendAttachment.colorWriteMask =
    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
    VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  // colorBlendAttachment.blendEnable = VK_FALSE;
  // colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
  // colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
  colorBlendAttachment.blendEnable = VK_TRUE;
  colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  colorBlendAttachment.dstColorBlendFactor =
   VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

  VkPipelineColorBlendStateCreateInfo colorBlending {};
  colorBlending.sType =
    VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendAttachment;
  colorBlending.blendConstants[0] = 0.0f; // Optional
  colorBlending.blendConstants[1] = 0.0f; // Optional
  colorBlending.blendConstants[2] = 0.0f; // Optional
  colorBlending.blendConstants[3] = 0.0f; // Optional

  VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 1;
  pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout_;
  pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
  pipelineLayoutInfo.pPushConstantRanges = NULL; // Optional
  VkResult result = vkCreatePipelineLayout (device_,
                                            &pipelineLayoutInfo,
                                            NULL,
                                            &pipelineLayout_);
  if (result != VK_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to create pipeline layout: \"%s\", returning\n"),
                ACE_TEXT (string_VkResult (result))));
    vkDestroyShaderModule (device_, fragShaderModule, NULL);
    vkDestroyShaderModule (device_, vertShaderModule, NULL);
    return;
  } // end IF

  VkPipelineDepthStencilStateCreateInfo depthStencil {};
  depthStencil.sType =
    VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencil.depthTestEnable = VK_TRUE;
  depthStencil.depthWriteEnable = VK_TRUE;
  depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
  depthStencil.depthBoundsTestEnable = VK_FALSE;
  depthStencil.minDepthBounds = 0.0f; // Optional
  depthStencil.maxDepthBounds = 1.0f; // Optional
  depthStencil.stencilTestEnable = VK_FALSE;
  depthStencil.front = {}; // Optional
  depthStencil.back = {};  // Optional

  VkGraphicsPipelineCreateInfo pipelineInfo {};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = &depthStencil;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = &dynamicState;
  pipelineInfo.layout = pipelineLayout_;
  pipelineInfo.renderPass = renderPass_;
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
  pipelineInfo.basePipelineIndex = -1;              // Optional

  result = vkCreateGraphicsPipelines (device_,
                                      VK_NULL_HANDLE,
                                      1,
                                      &pipelineInfo,
                                      NULL,
                                      &graphicsPipeline_);
  if (result != VK_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to create graphics pipeline: \"%s\", returning\n"),
                ACE_TEXT (string_VkResult (result))));
    vkDestroyShaderModule (device_, fragShaderModule, NULL);
    vkDestroyShaderModule (device_, vertShaderModule, NULL);
    return;
  } // end IF

  vkDestroyShaderModule (device_, fragShaderModule, NULL);
  vkDestroyShaderModule (device_, vertShaderModule, NULL);
}

void
HelloTriangleApplication::createFramebuffers ()
{
  swapChainFramebuffers_.resize (swapChainImageViews_.size ());

  VkResult result;
  for (size_t i = 0; i < swapChainImageViews_.size (); i++)
  {
    std::array<VkImageView, 2> attachments = { swapChainImageViews_[i],
                                               depthImageView_ };

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass_;
    framebufferInfo.attachmentCount = static_cast<uint32_t> (attachments.size ());
    framebufferInfo.pAttachments = attachments.data ();
    framebufferInfo.width = swapChainExtent_.width;
    framebufferInfo.height = swapChainExtent_.height;
    framebufferInfo.layers = 1;

    result = vkCreateFramebuffer (device_,
                                  &framebufferInfo,
                                  NULL,
                                  &swapChainFramebuffers_[i]);
    if (result != VK_SUCCESS)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to create framebuffer: \"%s\", returning\n"),
                  ACE_TEXT (string_VkResult (result))));
      return;
    } // end IF
  } // end FOR
}

void
HelloTriangleApplication::createImage (uint32_t width,
                                       uint32_t height,
                                       VkFormat format,
                                       VkImageTiling tiling,
                                       VkImageUsageFlags usage,
                                       VkMemoryPropertyFlags properties,
                                       VkImage& image,
                                       VkDeviceMemory& imageMemory)
{
  VkImageCreateInfo imageInfo {};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.extent.width = width;
  imageInfo.extent.height = height;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;
  imageInfo.format = format;
  imageInfo.tiling = tiling;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.usage = usage;
  imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VkResult result = vkCreateImage (device_,
                                    &imageInfo,
                                    NULL,
                                    &image);
  if (result != VK_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to create image: \"%s\", returning\n"),
                ACE_TEXT (string_VkResult (result))));
    return;
  } // end IF

  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements (device_, image, &memRequirements);

  VkMemoryAllocateInfo allocInfo {};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = findMemoryType (memRequirements.memoryTypeBits, properties);

  result = vkAllocateMemory (device_,
                              &allocInfo,
                              NULL,
                              &imageMemory);
  if (result != VK_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to allocate image memory: \"%s\", returning\n"),
                ACE_TEXT (string_VkResult (result))));
    return;
  } // end IF

  vkBindImageMemory (device_, image, imageMemory, 0);
}

void
HelloTriangleApplication::createTextureImage ()
{
  int texWidth, texHeight, texChannels;
  std::string texture_file_path = Common_File_Tools::getWorkingDirectory ();
  texture_file_path += ACE_DIRECTORY_SEPARATOR_STR_A;
  texture_file_path += ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_DATA_SUBDIRECTORY);
  texture_file_path += ACE_DIRECTORY_SEPARATOR_STR_A;
  //texture_file_path += ACE_TEXT_ALWAYS_CHAR (TEST_I_VULKAN_TEXTURE_FILENAME);
  texture_file_path += ACE_TEXT_ALWAYS_CHAR (TEST_I_VULKAN_MODEL_TEXTURE_FILENAME);
  stbi_uc* pixels = stbi_load (texture_file_path.c_str (),
                               &texWidth, &texHeight, &texChannels,
                               STBI_rgb_alpha);
  if (!pixels)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to stbi_load(): \"%s\", returning\n"),
                ACE_TEXT (texture_file_path.c_str ())));
    return;
  } // end IF
  VkDeviceSize imageSize = texWidth * texHeight * 4;

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  createBuffer (imageSize,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                stagingBuffer,
                stagingBufferMemory);

  void* data;
  VkResult result = vkMapMemory (device_,
                                 stagingBufferMemory,
                                 0,
                                 imageSize,
                                 0,
                                 &data);
  if (result != VK_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to vkMapMemory(): \"%s\", returning\n"),
                ACE_TEXT (string_VkResult (result))));
    stbi_image_free (pixels); pixels = NULL;
    return;
  } // end IF
  ACE_OS::memcpy (data, pixels, static_cast<size_t> (imageSize));
  vkUnmapMemory (device_, stagingBufferMemory);

  stbi_image_free (pixels); pixels = NULL;

  createImage (texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB,
               VK_IMAGE_TILING_OPTIMAL,
               VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
               textureImage_,
               textureImageMemory_);

  transitionImageLayout (textureImage_,
                         VK_FORMAT_R8G8B8A8_SRGB,
                         VK_IMAGE_LAYOUT_UNDEFINED,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

  copyBufferToImage (stagingBuffer,
                     textureImage_,
                     static_cast<uint32_t> (texWidth),
                     static_cast<uint32_t> (texHeight));

  transitionImageLayout (textureImage_,
                         VK_FORMAT_R8G8B8A8_SRGB,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                         VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  vkDestroyBuffer (device_, stagingBuffer, NULL);
  vkFreeMemory (device_, stagingBufferMemory, NULL);
}

VkImageView
HelloTriangleApplication::createImageView (VkImage image,
                                           VkFormat format,
                                           VkImageAspectFlags aspectFlags)
{
  VkImageView imageView;

  VkImageViewCreateInfo viewInfo {};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = image;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = format;
  viewInfo.subresourceRange.aspectMask = aspectFlags;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = 1;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;

  VkResult result = vkCreateImageView (device_,
                                       &viewInfo,
                                       NULL,
                                       &imageView);
  if (result != VK_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to vkCreateImageView(): \"%s\", aborting\n"),
                ACE_TEXT (string_VkResult (result))));
    return VK_NULL_HANDLE;
  } // end IF

  return imageView;
}

void
HelloTriangleApplication::createTextureImageView ()
{
  textureImageView_ = createImageView (textureImage_, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
}

void
HelloTriangleApplication::createTextureSampler ()
{
  VkSamplerCreateInfo samplerInfo {};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter = VK_FILTER_LINEAR;
  samplerInfo.minFilter = VK_FILTER_LINEAR;
  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.anisotropyEnable = VK_TRUE;

  VkPhysicalDeviceProperties properties {};
  vkGetPhysicalDeviceProperties (physicalDevice_, &properties);
  // samplerInfo.anisotropyEnable = VK_TRUE;
  // samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
  samplerInfo.anisotropyEnable = VK_FALSE; // disable anisotropic sampling
  samplerInfo.maxAnisotropy = 1.0f;

  samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;
  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  samplerInfo.mipLodBias = 0.0f;
  samplerInfo.minLod = 0.0f;
  samplerInfo.maxLod = 0.0f;

  VkResult result = vkCreateSampler (device_,
                                     &samplerInfo,
                                     NULL,
                                     &textureSampler_);
  if (result != VK_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to create texture sampler: \"%s\", returning\n"),
                ACE_TEXT (string_VkResult (result))));
    return;
  } // end IF
}

VkCommandBuffer
HelloTriangleApplication::beginSingleTimeCommands ()
{
  VkCommandBufferAllocateInfo allocInfo {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = commandPool_;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  VkResult result = vkAllocateCommandBuffers (device_,
                                              &allocInfo,
                                              &commandBuffer);
  if (result != VK_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to vkAllocateCommandBuffers(): \"%s\", aborting\n"),
                ACE_TEXT (string_VkResult (result))));
    return VK_NULL_HANDLE;
  } // end IF

  VkCommandBufferBeginInfo beginInfo {};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  result = vkBeginCommandBuffer (commandBuffer, &beginInfo);
  if (result != VK_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to vkBeginCommandBuffer(): \"%s\", aborting\n"),
                ACE_TEXT (string_VkResult (result))));
    return VK_NULL_HANDLE;
  } // end IF

  return commandBuffer;
}

void
HelloTriangleApplication::endSingleTimeCommands (VkCommandBuffer commandBuffer)
{
  VkResult result = vkEndCommandBuffer (commandBuffer);
  if (result != VK_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to vkEndCommandBuffer(): \"%s\", returning\n"),
                ACE_TEXT (string_VkResult (result))));
    return;
  } // end IF

  VkSubmitInfo submitInfo {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  result = vkQueueSubmit (graphicsQueue_,
                          1,
                          &submitInfo,
                          VK_NULL_HANDLE);
  if (result != VK_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to vkQueueSubmit(): \"%s\", returning\n"),
                ACE_TEXT (string_VkResult (result))));
    return;
  } // end IF

  result = vkQueueWaitIdle (graphicsQueue_);
  if (result != VK_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to vkQueueWaitIdle(): \"%s\", returning\n"),
                ACE_TEXT (string_VkResult (result))));
    return;
  } // end IF

  vkFreeCommandBuffers (device_,
                        commandPool_,
                        1,
                        &commandBuffer);
}

void
HelloTriangleApplication::transitionImageLayout (VkImage image,
                                                 VkFormat format,
                                                 VkImageLayout oldLayout,
                                                 VkImageLayout newLayout)
{
  VkCommandBuffer commandBuffer = beginSingleTimeCommands ();

  VkImageMemoryBarrier barrier {};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = oldLayout;
  barrier.newLayout = newLayout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = image;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;
  barrier.srcAccessMask = 0; // see below
  barrier.dstAccessMask = 0; // see below

  if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
  {
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

    if (hasStencilComponent (format))
      barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
  } // end IF
  else
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

  VkPipelineStageFlags sourceStage;
  VkPipelineStageFlags destinationStage;

  if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
      newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
  {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } // end IF
  else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
           newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
  {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } // end ELSE IF
  else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
           newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
  {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  } // end ELSE IF
  else
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("unsupported layout transition, returning\n")));
    return;
  } // end ELSE

  vkCmdPipelineBarrier (commandBuffer,
                        sourceStage, destinationStage,
                        0,
                        0, NULL,
                        0, NULL,
                        1, &barrier);

  endSingleTimeCommands (commandBuffer);
}

void
HelloTriangleApplication::copyBufferToImage (VkBuffer buffer,
                                             VkImage image,
                                             uint32_t width,
                                             uint32_t height)
{
  VkCommandBuffer commandBuffer = beginSingleTimeCommands ();

  VkBufferImageCopy region {};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;

  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = 1;

  region.imageOffset = {0, 0, 0};
  region.imageExtent = {width, height, 1};

  vkCmdCopyBufferToImage (commandBuffer,
                          buffer,
                          image,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          1,
                          &region);

  endSingleTimeCommands (commandBuffer);
}

void
HelloTriangleApplication::createCommandPool ()
{
  QueueFamilyIndices queueFamilyIndices = findQueueFamilies (physicalDevice_);

  VkCommandPoolCreateInfo poolInfo {};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value ();

  VkResult result = vkCreateCommandPool (device_,
                                         &poolInfo,
                                         NULL,
                                         &commandPool_);
  if (result != VK_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to create command pool: \"%s\", returning\n"),
                ACE_TEXT (string_VkResult (result))));
    return;
  } // end IF
}

VkFormat
HelloTriangleApplication::findSupportedFormat (const std::vector<VkFormat>& candidates,
                                               VkImageTiling tiling,
                                               VkFormatFeatureFlags features)
{
  for (VkFormat format: candidates)
  {
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties (physicalDevice_, format, &props);

    if ((tiling == VK_IMAGE_TILING_LINEAR) && (props.linearTilingFeatures & features) == features)
      return format;
    else if ((tiling == VK_IMAGE_TILING_OPTIMAL) && (props.optimalTilingFeatures & features) == features)
      return format;
  } // end FOR
  ACE_DEBUG ((LM_ERROR,
              ACE_TEXT ("failed to find supported format, aborting\n")));

  return VK_FORMAT_UNDEFINED;
}

VkFormat
HelloTriangleApplication::findDepthFormat ()
{
  return findSupportedFormat ({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                              VK_IMAGE_TILING_OPTIMAL,
                              VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

bool
HelloTriangleApplication::hasStencilComponent (VkFormat format)
{
  return format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
         format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void
HelloTriangleApplication::createDepthResources ()
{
  VkFormat depthFormat = findDepthFormat ();

  createImage (swapChainExtent_.width, swapChainExtent_.height, depthFormat,
               VK_IMAGE_TILING_OPTIMAL,
               VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
               depthImage_, depthImageMemory_);
  depthImageView_ =
    createImageView (depthImage_, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

  transitionImageLayout (depthImage_,
                         depthFormat,
                         VK_IMAGE_LAYOUT_UNDEFINED,
                         VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

void
HelloTriangleApplication::loadModel ()
{
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;
  std::string obj_file_path = Common_File_Tools::getWorkingDirectory ();
  obj_file_path += ACE_DIRECTORY_SEPARATOR_STR_A;
  obj_file_path += ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_DATA_SUBDIRECTORY);
  obj_file_path += ACE_DIRECTORY_SEPARATOR_STR_A;
  obj_file_path += ACE_TEXT_ALWAYS_CHAR (TEST_I_VULKAN_MODEL_FILENAME);

  if (!tinyobj::LoadObj (&attrib, &shapes, &materials, &warn, &err,
                         obj_file_path.c_str ()))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to tinyobj::LoadObj(\"%s\"): \"%s\", returning\n"),
                ACE_TEXT (obj_file_path.c_str ()),
                ACE_TEXT (err.c_str ())));
    return;
  } // end IF

  std::unordered_map<Vertex, uint32_t> uniqueVertices {};
  for (const auto& shape : shapes)
    for (const auto& index : shape.mesh.indices)
    {
      Vertex vertex {};

      vertex.pos = {attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]};

      vertex.texCoord = {attrib.texcoords[2 * index.texcoord_index + 0],
                         1.0f - attrib.texcoords[2 * index.texcoord_index + 1]};

      vertex.color = {1.0f, 1.0f, 1.0f};

      // vertices_.push_back (vertex);
      // indices_.push_back (indices_.size ());
      if (uniqueVertices.count (vertex) == 0)
      {
        uniqueVertices[vertex] = static_cast<uint32_t> (vertices_.size ());
        vertices_.push_back (vertex);
      } // end IF
      indices_.push_back (uniqueVertices[vertex]);
    } // end FOR
}

uint32_t
HelloTriangleApplication::findMemoryType (uint32_t typeFilter,
                                          VkMemoryPropertyFlags properties)
{
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties (physicalDevice_, &memProperties);

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
      return i;

  throw std::runtime_error ("failed to find suitable memory type!");
}

void
HelloTriangleApplication::createBuffer (VkDeviceSize size,
                                        VkBufferUsageFlags usage,
                                        VkMemoryPropertyFlags properties,
                                        VkBuffer& buffer,
                                        VkDeviceMemory& bufferMemory)
{
  VkBufferCreateInfo bufferInfo {};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VkResult result = vkCreateBuffer (device_,
                                    &bufferInfo,
                                    NULL,
                                    &buffer);
  if (result != VK_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to create vertex buffer: \"%s\", returning\n"),
                ACE_TEXT (string_VkResult (result))));
    return;
  } // end IF

  VkMemoryAllocateInfo allocInfo {};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements (device_,
                                 buffer,
                                 &memRequirements);
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = findMemoryType (memRequirements.memoryTypeBits, properties);

  result = vkAllocateMemory (device_,
                             &allocInfo,
                             NULL,
                             &bufferMemory);
  if (result != VK_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to allocate vertex buffer memory: \"%s\", returning\n"),
                ACE_TEXT (string_VkResult (result))));
    return;
  } // end IF

  result = vkBindBufferMemory (device_,
                               buffer,
                               bufferMemory,
                               0);
  if (result != VK_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to vkBindBufferMemory(): \"%s\", returning\n"),
                ACE_TEXT (string_VkResult (result))));
    return;
  } // end IF
}

void
HelloTriangleApplication::copyBuffer (VkBuffer srcBuffer,
                                      VkBuffer dstBuffer,
                                      VkDeviceSize size)
{
  VkCommandBuffer commandBuffer = beginSingleTimeCommands ();

  VkBufferCopy copyRegion {};
  copyRegion.srcOffset = 0; // Optional
  copyRegion.dstOffset = 0; // Optional
  copyRegion.size = size;
  vkCmdCopyBuffer (commandBuffer,
                   srcBuffer,
                   dstBuffer,
                   1,
                   &copyRegion);

 endSingleTimeCommands (commandBuffer);
}

void
HelloTriangleApplication::createVertexBuffer ()
{
  VkDeviceSize bufferSize = sizeof (vertices_[0]) * vertices_.size ();
  createBuffer (bufferSize,
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                vertexBuffer_, vertexBufferMemory_);

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  createBuffer (bufferSize,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                stagingBuffer, stagingBufferMemory);

  void* data;
  VkResult result = vkMapMemory (device_,
                                 stagingBufferMemory,
                                 0,
                                 bufferSize,
                                 0,
                                 &data);
  if (result != VK_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to vkMapMemory(): \"%s\", returning\n"),
                ACE_TEXT (string_VkResult (result))));
    return;
  } // end IF
  ACE_OS::memcpy (data, vertices_.data (), (size_t)bufferSize);
  vkUnmapMemory (device_, stagingBufferMemory);

  createBuffer (bufferSize,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                vertexBuffer_,
                vertexBufferMemory_);

  copyBuffer (stagingBuffer, vertexBuffer_, bufferSize);

  vkDestroyBuffer (device_, stagingBuffer, NULL);
  vkFreeMemory (device_, stagingBufferMemory, NULL);
}

void
HelloTriangleApplication::createIndexBuffer ()
{
  VkDeviceSize bufferSize = sizeof (indices_[0]) * indices_.size ();

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  createBuffer (bufferSize,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                stagingBuffer,
                stagingBufferMemory);

  void* data;
  VkResult result = vkMapMemory (device_,
                                 stagingBufferMemory,
                                 0,
                                 bufferSize,
                                 0,
                                 &data);
  if (result != VK_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to vkMapMemory(): \"%s\", returning\n"),
                ACE_TEXT (string_VkResult (result))));
    return;
  } // end IF
  ACE_OS::memcpy (data, indices_.data (), (size_t)bufferSize);
  vkUnmapMemory (device_, stagingBufferMemory);

  createBuffer (bufferSize,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                indexBuffer_,
                indexBufferMemory_);

  copyBuffer (stagingBuffer, indexBuffer_, bufferSize);

  vkDestroyBuffer (device_, stagingBuffer, NULL);
  vkFreeMemory (device_, stagingBufferMemory, NULL);
}

void
HelloTriangleApplication::createUniformBuffers ()
{
  VkDeviceSize bufferSize = sizeof (UniformBufferObject);

  uniformBuffers_.resize (TEST_I_VULKAN_MAX_FRAMES_IN_FLIGHT);
  uniformBuffersMemory_.resize (TEST_I_VULKAN_MAX_FRAMES_IN_FLIGHT);
  uniformBuffersMapped_.resize (TEST_I_VULKAN_MAX_FRAMES_IN_FLIGHT);

  VkResult result;
  for (size_t i = 0; i < TEST_I_VULKAN_MAX_FRAMES_IN_FLIGHT; i++)
  {
    createBuffer (bufferSize,
                  VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                  uniformBuffers_[i],
                  uniformBuffersMemory_[i]);

    result = vkMapMemory (device_,
                          uniformBuffersMemory_[i],
                          0,
                          bufferSize,
                          0,
                          &uniformBuffersMapped_[i]);
    if (result != VK_SUCCESS)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to vkMapMemory(): \"%s\", returning\n"),
                  ACE_TEXT (string_VkResult (result))));
      return;
    } // end IF
  } // end FOR
}

void
HelloTriangleApplication::createDescriptorPool ()
{
  std::array<VkDescriptorPoolSize, 2> poolSizes {};
  poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[0].descriptorCount =
    static_cast<uint32_t> (TEST_I_VULKAN_MAX_FRAMES_IN_FLIGHT);
  poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSizes[1].descriptorCount =
    static_cast<uint32_t> (TEST_I_VULKAN_MAX_FRAMES_IN_FLIGHT);

  VkDescriptorPoolCreateInfo poolInfo {};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = static_cast<uint32_t> (poolSizes.size ());
  poolInfo.pPoolSizes = poolSizes.data ();
  poolInfo.maxSets = static_cast<uint32_t> (TEST_I_VULKAN_MAX_FRAMES_IN_FLIGHT);

  VkResult result = vkCreateDescriptorPool (device_,
                                            &poolInfo,
                                            NULL,
                                            &descriptorPool_);
  if (result != VK_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to create decscriptor pool: \"%s\", returning\n"),
                ACE_TEXT (string_VkResult (result))));
    return;
  } // end IF
}

void
HelloTriangleApplication::createDescriptorSets ()
{
  std::vector<VkDescriptorSetLayout> layouts (TEST_I_VULKAN_MAX_FRAMES_IN_FLIGHT, descriptorSetLayout_);

  VkDescriptorSetAllocateInfo allocInfo {};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPool_;
  allocInfo.descriptorSetCount =
    static_cast<uint32_t> (TEST_I_VULKAN_MAX_FRAMES_IN_FLIGHT);
  allocInfo.pSetLayouts = layouts.data ();

  descriptorSets_.resize (TEST_I_VULKAN_MAX_FRAMES_IN_FLIGHT);
  VkResult result = vkAllocateDescriptorSets (device_,
                                              &allocInfo,
                                              descriptorSets_.data ());
  if (result != VK_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to allocate decscriptor sets: \"%s\", returning\n"),
                ACE_TEXT (string_VkResult (result))));
    return;
  } // end IF

  for (size_t i = 0; i < TEST_I_VULKAN_MAX_FRAMES_IN_FLIGHT; i++)
  {
    VkDescriptorBufferInfo bufferInfo {};
    bufferInfo.buffer = uniformBuffers_[i];
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof (UniformBufferObject);

    VkDescriptorImageInfo imageInfo {};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = textureImageView_;
    imageInfo.sampler = textureSampler_;

    std::array<VkWriteDescriptorSet, 2> descriptorWrites {};

    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = descriptorSets_[i];
    descriptorWrites[0].dstBinding = 0;
    descriptorWrites[0].dstArrayElement = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pBufferInfo = &bufferInfo;

    descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet = descriptorSets_[i];
    descriptorWrites[1].dstBinding = 1;
    descriptorWrites[1].dstArrayElement = 0;
    descriptorWrites[1].descriptorType =
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].pImageInfo = &imageInfo;
 
    vkUpdateDescriptorSets (device_,
                            static_cast<uint32_t> (descriptorWrites.size ()),
                            descriptorWrites.data (),
                            0,
                            NULL);
  } // end FOR
}

void
HelloTriangleApplication::createCommandBuffers ()
{
  commandBuffers_.resize (TEST_I_VULKAN_MAX_FRAMES_IN_FLIGHT);

  VkCommandBufferAllocateInfo allocInfo {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = commandPool_;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = (uint32_t)commandBuffers_.size ();

  VkResult result = vkAllocateCommandBuffers (device_,
                                              &allocInfo,
                                              commandBuffers_.data ());
  if (result != VK_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to allocate command buffer(s): \"%s\", returning\n"),
                ACE_TEXT (string_VkResult (result))));
    return;
  } // end IF
}

void
HelloTriangleApplication::recordCommandBuffer (VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
  VkCommandBufferBeginInfo beginInfo {};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = 0;               // Optional
  beginInfo.pInheritanceInfo = NULL; // Optional

  VkResult result = vkBeginCommandBuffer (commandBuffer,
                                          &beginInfo);
  if (result != VK_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to begin command buffer: \"%s\", returning\n"),
                ACE_TEXT (string_VkResult (result))));
    return;
  } // end IF

  std::array<VkClearValue, 2> clearValues {};
  clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
  clearValues[1].depthStencil = {1.0f, 0};

  VkRenderPassBeginInfo renderPassInfo {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = renderPass_;
  renderPassInfo.framebuffer = swapChainFramebuffers_[currentFrame_];
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = swapChainExtent_;
  renderPassInfo.clearValueCount = static_cast<uint32_t> (clearValues.size ());
  renderPassInfo.pClearValues = clearValues.data ();

  vkCmdBeginRenderPass (commandBuffer,
                        &renderPassInfo,
                        VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline (commandBuffer,
                     VK_PIPELINE_BIND_POINT_GRAPHICS,
                     graphicsPipeline_);

  VkViewport viewport {};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float> (swapChainExtent_.width);
  viewport.height = static_cast<float> (swapChainExtent_.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport (commandBuffer, 0, 1, &viewport);

  VkRect2D scissor {};
  scissor.offset = {0, 0};
  scissor.extent = swapChainExtent_;
  vkCmdSetScissor (commandBuffer, 0, 1, &scissor);

  VkBuffer vertexBuffers[] = { vertexBuffer_ };
  VkDeviceSize offsets[] = { 0 };
  vkCmdBindVertexBuffers (commandBuffer,
                          0,
                          1,
                          vertexBuffers,
                          offsets);
  vkCmdBindIndexBuffer (commandBuffer,
                        indexBuffer_,
                        0,
                        VK_INDEX_TYPE_UINT32);

  //vkCmdDraw (commandBuffer,
  //           static_cast<uint32_t> (vertices.size ()),
  //           1,
  //           0,
  //           0);

  vkCmdBindDescriptorSets (commandBuffer,
                           VK_PIPELINE_BIND_POINT_GRAPHICS,
                           pipelineLayout_,
                           0,
                           1,
                           &descriptorSets_[currentFrame_],
                           0,
                           NULL);

  vkCmdDrawIndexed (commandBuffer,
                    static_cast<uint32_t> (indices_.size ()),
                    1,
                    0,
                    0,
                    0);

  vkCmdEndRenderPass (commandBuffer);

  result = vkEndCommandBuffer (commandBuffer);
  if (result != VK_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to end command buffer: \"%s\", returning\n"),
                ACE_TEXT (string_VkResult (result))));
    return;
  } // end IF
}

void
HelloTriangleApplication::createSyncObjects ()
{
  imageAvailableSemaphores_.resize (TEST_I_VULKAN_MAX_FRAMES_IN_FLIGHT);
  renderFinishedSemaphores_.resize (TEST_I_VULKAN_MAX_FRAMES_IN_FLIGHT);
  inFlightFences_.resize (TEST_I_VULKAN_MAX_FRAMES_IN_FLIGHT);

  VkSemaphoreCreateInfo semaphoreInfo {};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo {};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  VkResult result;
  for (int i = 0; i < TEST_I_VULKAN_MAX_FRAMES_IN_FLIGHT; i++)
  {
    result = vkCreateSemaphore (device_,
                                &semaphoreInfo,
                                NULL,
                                &imageAvailableSemaphores_[i]);
    if (result != VK_SUCCESS)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to create semaphore: \"%s\", returning\n"),
                  ACE_TEXT (string_VkResult (result))));
      return;
    } // end IF
  } // end FOR
  for (int i = 0; i < TEST_I_VULKAN_MAX_FRAMES_IN_FLIGHT; i++)
  {
    result = vkCreateSemaphore (device_,
                                &semaphoreInfo,
                                NULL,
                                &renderFinishedSemaphores_[i]);
    if (result != VK_SUCCESS)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to create semaphore: \"%s\", returning\n"),
                  ACE_TEXT (string_VkResult (result))));
      return;
    } // end IF
  } // end FOR
  for (int i = 0; i < TEST_I_VULKAN_MAX_FRAMES_IN_FLIGHT; i++)
  {
    result = vkCreateFence (device_,
                            &fenceInfo,
                            NULL, &inFlightFences_[i]);
    if (result != VK_SUCCESS)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to create fence: \"%s\", returning\n"),
                  ACE_TEXT (string_VkResult (result))));
      return;
    } // end IF
  } // end FOR
}

void
HelloTriangleApplication::updateUniformBuffer (uint32_t currentImage)
{
  static auto startTime = std::chrono::high_resolution_clock::now ();
  auto currentTime = std::chrono::high_resolution_clock::now ();
  float time =
    std::chrono::duration<float, std::chrono::seconds::period> (currentTime - startTime).count ();

  UniformBufferObject ubo {};
  ubo.model =
    glm::rotate (glm::mat4 (1.0f), time * glm::radians (90.0f), glm::vec3 (0.0f, 0.0f, 1.0f));
  ubo.view =
    glm::lookAt (glm::vec3 (2.0f, 2.0f, 2.0f), glm::vec3 (0.0f, 0.0f, 0.0f), glm::vec3 (0.0f, 0.0f, 1.0f));
  ubo.proj =
    glm::perspective (glm::radians (45.0f), swapChainExtent_.width / (float) swapChainExtent_.height, 0.1f, 10.0f);
  ubo.proj[1][1] *= -1;
  ACE_OS::memcpy (uniformBuffersMapped_[currentImage], &ubo, sizeof (UniformBufferObject));
}

void
HelloTriangleApplication::drawFrame ()
{
  vkWaitForFences (device_, 1, &inFlightFences_[currentFrame_], VK_TRUE, UINT64_MAX);
  //vkResetFences (device_, 1, &inFlightFences_[currentFrame_]);

  uint32_t imageIndex;
  VkResult result = vkAcquireNextImageKHR (device_,
                                           swapChain_,
                                           UINT64_MAX,
                                           imageAvailableSemaphores_[currentFrame_],
                                           VK_NULL_HANDLE,
                                           &imageIndex);
  if (result == VK_ERROR_OUT_OF_DATE_KHR)
  {
    recreateSwapChain ();
    return;
  } // end IF
  else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to vkAcquireNextImageKHR(): \"%s\", returning\n"),
                ACE_TEXT (string_VkResult (result))));
    return;
  } // end ELSE IF

  // Only reset the fence if we are submitting work
  vkResetFences (device_, 1, &inFlightFences_[currentFrame_]);

  updateUniformBuffer (currentFrame_);

  vkResetCommandBuffer (commandBuffers_[currentFrame_], 0);
  recordCommandBuffer (commandBuffers_[currentFrame_], imageIndex);

  VkSubmitInfo submitInfo {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  VkSemaphore waitSemaphores[] = { imageAvailableSemaphores_[currentFrame_] };
  VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffers_[currentFrame_];
  VkSemaphore signalSemaphores[] = { renderFinishedSemaphores_[currentFrame_] };
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  result = vkQueueSubmit (graphicsQueue_,
                          1,
                          &submitInfo,
                          inFlightFences_[currentFrame_]);
  if (result != VK_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to vkQueueSubmit(): \"%s\", returning\n"),
                ACE_TEXT (string_VkResult (result))));
    return;
  } // end IF

  VkPresentInfoKHR presentInfo {};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;
  VkSwapchainKHR swapChains[] = { swapChain_ };
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &imageIndex;
  presentInfo.pResults = NULL; // Optional

  result = vkQueuePresentKHR (presentQueue_, &presentInfo);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized_)
  {
    framebufferResized_ = false;
    recreateSwapChain ();
  } // end IF
  else if (result != VK_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to vkQueuePresentKHR(): \"%s\", returning\n"),
                ACE_TEXT (string_VkResult (result))));
    return;
  } // end ELSE IF

  currentFrame_ = (currentFrame_ + 1) % TEST_I_VULKAN_MAX_FRAMES_IN_FLIGHT;
}

void
HelloTriangleApplication::mainLoop ()
{
  while (!glfwWindowShouldClose (window_))
  {
    glfwPollEvents();
    drawFrame ();
  } // end WHILE

  vkDeviceWaitIdle (device_);
}

void
HelloTriangleApplication::cleanupSwapChain ()
{
  vkDestroyImageView (device_, depthImageView_, NULL);
  vkDestroyImage (device_, depthImage_, NULL);
  vkFreeMemory (device_, depthImageMemory_, NULL);

  for (size_t i = 0; i < swapChainFramebuffers_.size (); i++)
    vkDestroyFramebuffer (device_, swapChainFramebuffers_[i], NULL);
  swapChainFramebuffers_.clear ();

  for (size_t i = 0; i < swapChainImageViews_.size (); i++)
    vkDestroyImageView (device_, swapChainImageViews_[i], NULL);
  swapChainImageViews_.clear ();

  vkDestroySwapchainKHR (device_, swapChain_, NULL);
}

void 
HelloTriangleApplication::recreateSwapChain ()
{
  int width = 0, height = 0;
  glfwGetFramebufferSize (window_, &width, &height);
  while (width == 0 || height == 0)
  {
    glfwGetFramebufferSize (window_, &width, &height);
    glfwWaitEvents ();
  } // end WHILE

  vkDeviceWaitIdle (device_);

  cleanupSwapChain ();

  createSwapChain ();
  createImageViews ();
  createDepthResources ();
  createFramebuffers ();
}

void
HelloTriangleApplication::cleanup (bool enableValidationLayers_in)
{
  for (int i = 0; i < TEST_I_VULKAN_MAX_FRAMES_IN_FLIGHT; i++)
  {
    vkDestroySemaphore (device_, renderFinishedSemaphores_[i], NULL);
    vkDestroySemaphore (device_, imageAvailableSemaphores_[i], NULL);
    vkDestroyFence (device_, inFlightFences_[i], NULL);
  } // end FOR
  vkDestroyCommandPool (device_, commandPool_, NULL);
  vkDestroyPipeline (device_, graphicsPipeline_, NULL);
  vkDestroyPipelineLayout (device_, pipelineLayout_, NULL);
  vkDestroyRenderPass (device_, renderPass_, NULL);
  cleanupSwapChain ();
  vkDestroySampler (device_, textureSampler_, NULL);
  vkDestroyImageView (device_, textureImageView_, NULL);
  vkDestroyImage (device_, textureImage_, NULL);
  vkFreeMemory (device_, textureImageMemory_, NULL);
  for (size_t i = 0; i < TEST_I_VULKAN_MAX_FRAMES_IN_FLIGHT; i++)
  {
    vkDestroyBuffer (device_, uniformBuffers_[i], NULL);
    vkUnmapMemory (device_, uniformBuffersMemory_[i]);
    vkFreeMemory (device_, uniformBuffersMemory_[i], NULL);
  } // end FOR
  uniformBuffers_.clear (); uniformBuffersMemory_.clear (); uniformBuffersMapped_.clear ();
  vkDestroyDescriptorPool (device_, descriptorPool_, NULL);
  vkDestroyDescriptorSetLayout (device_, descriptorSetLayout_, NULL);
  vkDestroyBuffer (device_, indexBuffer_, NULL);
  vkFreeMemory (device_, indexBufferMemory_, NULL);
  vkDestroyBuffer (device_, vertexBuffer_, NULL);
  vkFreeMemory (device_, vertexBufferMemory_, NULL);
  vkDestroyDevice (device_, NULL);

  if (enableValidationLayers_in)
  {
    DestroyDebugUtilsMessengerEXT (instance_, debugMessenger_, NULL);
    debugMessenger_ = VK_NULL_HANDLE;
  } // end IF

  vkDestroySurfaceKHR (instance_, surface_, NULL); surface_ = VK_NULL_HANDLE;
  vkDestroyInstance (instance_, NULL); instance_ = VK_NULL_HANDLE;

  glfwDestroyWindow (window_); window_ = NULL;
}
