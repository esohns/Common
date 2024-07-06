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

#include <optional>
#include <vector>

#if defined (GLFW_SUPPORT)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#define VK_USE_PLATFORM_WIN32_KHR
//#elif defined (ACE_LINUX)
#endif // ACE_WIN32 || ACE_WIN64 || ACE_LINUX
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#endif // GLFW_SUPPORT

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback (VkDebugUtilsMessageSeverityFlagBitsEXT,
                                                     VkDebugUtilsMessageTypeFlagsEXT,
                                                     const VkDebugUtilsMessengerCallbackDataEXT*,
                                                     void*);

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
  void createGraphicsPipeline ();
  void createFramebuffers ();
  void createCommandPool ();
  void createCommandBuffers ();
  void recordCommandBuffer (VkCommandBuffer, uint32_t);
  void createSyncObjects ();

  void drawFrame ();
  void mainLoop ();

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

  uint32_t                     currentFrame_;

  GLFWwindow*                  window_;
};

#endif
