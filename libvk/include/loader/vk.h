
#pragma once

#ifndef _LIBVK_VK_H_
#define _LIBVK_VK_H_

#include <vulkan.h>

#define VK_DECLEARE_FUNCTION(name__) extern PFN_##name__ name__

VK_DECLEARE_FUNCTION(vkCreateInstance);
VK_DECLEARE_FUNCTION(vkGetInstanceProcAddr);
VK_DECLEARE_FUNCTION(vkEnumerateInstanceVersion);
VK_DECLEARE_FUNCTION(vkEnumerateInstanceExtensionProperties);
VK_DECLEARE_FUNCTION(vkEnumerateInstanceLayerProperties);
VK_DECLEARE_FUNCTION(vkDestroyInstance);
VK_DECLEARE_FUNCTION(vkCreateWin32SurfaceKHR);
VK_DECLEARE_FUNCTION(vkDestroySurfaceKHR);
VK_DECLEARE_FUNCTION(vkEnumeratePhysicalDevices);
VK_DECLEARE_FUNCTION(vkGetPhysicalDeviceProperties);
VK_DECLEARE_FUNCTION(vkGetPhysicalDeviceProperties2);
VK_DECLEARE_FUNCTION(vkGetPhysicalDeviceProperties2KHR);
VK_DECLEARE_FUNCTION(vkGetPhysicalDeviceQueueFamilyProperties);
VK_DECLEARE_FUNCTION(vkGetPhysicalDeviceSurfaceSupportKHR);
VK_DECLEARE_FUNCTION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
VK_DECLEARE_FUNCTION(vkGetPhysicalDeviceWin32PresentationSupportKHR);
VK_DECLEARE_FUNCTION(vkGetPhysicalDeviceSurfaceFormatsKHR);
VK_DECLEARE_FUNCTION(vkGetPhysicalDeviceSurfacePresentModesKHR);
VK_DECLEARE_FUNCTION(vkCreateSwapchainKHR);
VK_DECLEARE_FUNCTION(vkDestroySwapchainKHR);
VK_DECLEARE_FUNCTION(vkAcquireNextImageKHR);
VK_DECLEARE_FUNCTION(vkQueuePresentKHR);
VK_DECLEARE_FUNCTION(vkGetSwapchainImagesKHR);
VK_DECLEARE_FUNCTION(vkCreateDevice);
VK_DECLEARE_FUNCTION(vkDestroyDevice);
VK_DECLEARE_FUNCTION(vkDeviceWaitIdle);
VK_DECLEARE_FUNCTION(vkEnumerateDeviceExtensionProperties);
VK_DECLEARE_FUNCTION(vkGetDeviceQueue);
VK_DECLEARE_FUNCTION(vkCreateCommandPool);
VK_DECLEARE_FUNCTION(vkDestroyCommandPool);
VK_DECLEARE_FUNCTION(vkTrimCommandPool);
VK_DECLEARE_FUNCTION(vkResetCommandPool);
VK_DECLEARE_FUNCTION(vkAllocateCommandBuffers);
VK_DECLEARE_FUNCTION(vkFreeCommandBuffers);
VK_DECLEARE_FUNCTION(vkResetCommandBuffer);
VK_DECLEARE_FUNCTION(vkBeginCommandBuffer);
VK_DECLEARE_FUNCTION(vkEndCommandBuffer);
VK_DECLEARE_FUNCTION(vkQueueSubmit);
VK_DECLEARE_FUNCTION(vkQueueWaitIdle);
VK_DECLEARE_FUNCTION(vkCmdExecuteCommands);
VK_DECLEARE_FUNCTION(vkCmdSetDeviceMask);
VK_DECLEARE_FUNCTION(vkCreateFence);
VK_DECLEARE_FUNCTION(vkDestroyFence);
VK_DECLEARE_FUNCTION(vkGetFenceStatus);
VK_DECLEARE_FUNCTION(vkResetFences);
VK_DECLEARE_FUNCTION(vkWaitForFences);
VK_DECLEARE_FUNCTION(vkCreateSemaphore);
VK_DECLEARE_FUNCTION(vkDestroySemaphore);
VK_DECLEARE_FUNCTION(vkCreateEvent);
VK_DECLEARE_FUNCTION(vkDestroyEvent);
VK_DECLEARE_FUNCTION(vkGetEventStatus);
VK_DECLEARE_FUNCTION(vkSetEvent);
VK_DECLEARE_FUNCTION(vkResetEvent);
VK_DECLEARE_FUNCTION(vkCmdPipelineBarrier);
VK_DECLEARE_FUNCTION(vkCmdClearColorImage);

#undef VK_DECLEARE_FUNCTION

#endif