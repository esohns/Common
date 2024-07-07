/***************************************************************************
 *   Copyright (C) 2010 by Erik Sohns   *
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

#ifndef TEST_I_APP_H
#define TEST_I_APP_H

#include <array>
#include <optional>
#include <vector>

#if defined (GLM_SUPPORT)
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#endif // GLM_SUPPORT

#include "ace/config-lite.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#define VK_USE_PLATFORM_WIN32_KHR
#elif defined (ACE_LINUX)
//#define VK_USE_PLATFORM_WAYLAND_KHR
#define VK_USE_PLATFORM_XLIB_KHR
#endif // ACE_WIN32 || ACE_WIN64 || ACE_LINUX
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

struct Vertex
{
  glm::vec3 pos;
  glm::vec3 color;
  glm::vec2 texCoord;

  static VkVertexInputBindingDescription getBindingDescription ()
  {
    VkVertexInputBindingDescription bindingDescription {};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof (Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
  }

  // static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions ()
  // {
  //  std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions {};
  //  attributeDescriptions[0].binding = 0;
  //  attributeDescriptions[0].location = 0;
  //  attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
  //  attributeDescriptions[0].offset = offsetof (Vertex, pos);

  //  attributeDescriptions[1].binding = 0;
  //  attributeDescriptions[1].location = 1;
  //  attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  //  attributeDescriptions[1].offset = offsetof (Vertex, color);

  //  return attributeDescriptions;
  // }

  static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions ()
  {
    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions {};

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof (Vertex, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof (Vertex, color);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof (Vertex, texCoord);

    return attributeDescriptions;
  }

  bool operator== (const Vertex& other) const
  {
    return pos == other.pos && color == other.color && texCoord == other.texCoord;
  }
};

///////////////////////////////////////////

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback (VkDebugUtilsMessageSeverityFlagBitsEXT,
                                                     VkDebugUtilsMessageTypeFlagsEXT,
                                                     const VkDebugUtilsMessengerCallbackDataEXT*,
                                                     void*);

///////////////////////////////////////////

class HelloTriangleApplication
{
 public:
  HelloTriangleApplication ();

  void run ()
  {
    bool enable_validation_layers_b =
#if defined (NDEBUG)
      false
#else
      true
#endif // NDEBUG
      ;

    initWindow ();
    initVulkan (enable_validation_layers_b);
    mainLoop ();
    cleanup (enable_validation_layers_b);
  }

  bool framebufferResized_;

 private:
  void initWindow ();

  bool checkValidationLayerSupport (const std::vector<const char*>&);
  std::vector<const char*> getRequiredExtensions (bool); // enable validation layers ?
  void createInstance (bool); // enable validation layers ?
  void initVulkan (bool); // enable validation layers ?
  void populateDebugMessengerCreateInfo (VkDebugUtilsMessengerCreateInfoEXT&);
  void setupDebugMessenger (bool); // enable validation layers ?
  bool checkDeviceExtensionSupport (VkPhysicalDevice);
  bool isDeviceSuitable (VkPhysicalDevice);
  void pickPhysicalDevice ();
  struct QueueFamilyIndices
  {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete ()
    {
      return graphicsFamily.has_value () && presentFamily.has_value ();
    }
  };
  QueueFamilyIndices findQueueFamilies (VkPhysicalDevice);
  struct SwapChainSupportDetails
  {
    VkSurfaceCapabilitiesKHR        capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR>   presentModes;
  };
  SwapChainSupportDetails querySwapChainSupport (VkPhysicalDevice);
  void createLogicalDevice (bool); // enable validation layers ?
  void createSurface ();
  VkSurfaceFormatKHR chooseSwapSurfaceFormat (const std::vector<VkSurfaceFormatKHR>&);
  VkPresentModeKHR chooseSwapPresentMode (const std::vector<VkPresentModeKHR>&);
  VkExtent2D chooseSwapExtent (const VkSurfaceCapabilitiesKHR&);
  void createSwapChain ();
  void createImageViews ();
  VkShaderModule createShaderModule (const std::vector<char>&);
  void createRenderPass ();
  void createDescriptorSetLayout ();
  void createGraphicsPipeline ();
  void createFramebuffers ();
  void createImage (uint32_t, uint32_t, VkFormat, VkImageTiling,
                    VkImageUsageFlags, VkMemoryPropertyFlags,
                    VkImage&, VkDeviceMemory&);
  void createTextureImage ();
  VkImageView createImageView (VkImage, VkFormat, VkImageAspectFlags);
  void createTextureImageView ();
  void createTextureSampler ();
  VkCommandBuffer beginSingleTimeCommands ();
  void endSingleTimeCommands (VkCommandBuffer);
  void transitionImageLayout (VkImage, VkFormat, VkImageLayout, VkImageLayout);
  void copyBufferToImage (VkBuffer, VkImage, uint32_t, uint32_t);
  void createCommandPool ();
  VkFormat findSupportedFormat (const std::vector<VkFormat>&, VkImageTiling, VkFormatFeatureFlags);
  VkFormat findDepthFormat ();
  bool hasStencilComponent (VkFormat);
  void createDepthResources ();
  void loadModel ();
  uint32_t findMemoryType (uint32_t, VkMemoryPropertyFlags);
  void createBuffer (VkDeviceSize, VkBufferUsageFlags, VkMemoryPropertyFlags,
                     VkBuffer&, VkDeviceMemory&);
  void copyBuffer (VkBuffer, VkBuffer, VkDeviceSize);
  void createVertexBuffer ();
  void createIndexBuffer ();
  void createUniformBuffers ();
  void createDescriptorPool ();
  void createDescriptorSets ();
  void createCommandBuffers ();
  void recordCommandBuffer (VkCommandBuffer, uint32_t);
  void createSyncObjects ();

  void updateUniformBuffer (uint32_t);
  void drawFrame ();
  void mainLoop ();

  void cleanupSwapChain ();
  void recreateSwapChain ();

  void cleanup (bool); // enable validation layers ?

  std::vector<VkCommandBuffer> commandBuffers_;
  VkCommandPool                commandPool_;
  VkDebugUtilsMessengerEXT     debugMessenger_;
  VkDevice                     device_;
  VkPipeline                   graphicsPipeline_;
  VkQueue                      graphicsQueue_;
  VkInstance                   instance_;
  VkPhysicalDevice             physicalDevice_;
  VkPipelineLayout             pipelineLayout_;
  VkQueue                      presentQueue_;
  VkRenderPass                 renderPass_;
  VkSurfaceKHR                 surface_;
  VkSwapchainKHR               swapChain_;
  VkExtent2D                   swapChainExtent_;
  std::vector<VkFramebuffer>   swapChainFramebuffers_;
  VkFormat                     swapChainImageFormat_;
  std::vector<VkImage>         swapChainImages_;
  std::vector<VkImageView>     swapChainImageViews_;

  std::vector<VkSemaphore>     imageAvailableSemaphores_;
  std::vector<VkSemaphore>     renderFinishedSemaphores_;
  std::vector<VkFence>         inFlightFences_;

  VkBuffer                     vertexBuffer_;
  VkDeviceMemory               vertexBufferMemory_;
  VkBuffer                     indexBuffer_;
  VkDeviceMemory               indexBufferMemory_;

  std::vector<VkBuffer>        uniformBuffers_;
  std::vector<VkDeviceMemory>  uniformBuffersMemory_;
  std::vector<void*>           uniformBuffersMapped_;

  VkDescriptorSetLayout        descriptorSetLayout_;
  VkDescriptorPool             descriptorPool_;
  std::vector<VkDescriptorSet> descriptorSets_;

  VkImage                      textureImage_;
  VkDeviceMemory               textureImageMemory_;
  VkImageView                  textureImageView_;
  VkSampler                    textureSampler_;

  VkImage                      depthImage_;
  VkDeviceMemory               depthImageMemory_;
  VkImageView                  depthImageView_;

  std::vector<Vertex>          vertices_;
  std::vector<uint32_t>        indices_;

  uint32_t                     currentFrame_;

  GLFWwindow*                  window_;
};

#endif
