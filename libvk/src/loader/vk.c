
#include <vulkan.h>
#include <loader/vk.h>

#define VK_DEFINE_FUNCTION(name__) PFN_##name__ name__ = NULL

VK_DEFINE_FUNCTION(vkCreateInstance);
VK_DEFINE_FUNCTION(vkGetInstanceProcAddr);
VK_DEFINE_FUNCTION(vkEnumerateInstanceVersion);
VK_DEFINE_FUNCTION(vkEnumerateInstanceExtensionProperties);
VK_DEFINE_FUNCTION(vkEnumerateInstanceLayerProperties);
VK_DEFINE_FUNCTION(vkDestroyInstance);
VK_DEFINE_FUNCTION(vkCreateWin32SurfaceKHR);
VK_DEFINE_FUNCTION(vkDestroySurfaceKHR);
VK_DEFINE_FUNCTION(vkEnumeratePhysicalDevices);
VK_DEFINE_FUNCTION(vkGetPhysicalDeviceProperties);
VK_DEFINE_FUNCTION(vkGetPhysicalDeviceProperties2);
VK_DEFINE_FUNCTION(vkGetPhysicalDeviceProperties2KHR);
VK_DEFINE_FUNCTION(vkGetPhysicalDeviceQueueFamilyProperties);
VK_DEFINE_FUNCTION(vkGetPhysicalDeviceSurfaceSupportKHR);
VK_DEFINE_FUNCTION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
VK_DEFINE_FUNCTION(vkGetPhysicalDeviceWin32PresentationSupportKHR);
VK_DEFINE_FUNCTION(vkGetPhysicalDeviceSurfaceFormatsKHR);
VK_DEFINE_FUNCTION(vkGetPhysicalDeviceSurfacePresentModesKHR);
VK_DEFINE_FUNCTION(vkCreateSwapchainKHR);
VK_DEFINE_FUNCTION(vkDestroySwapchainKHR);
VK_DEFINE_FUNCTION(vkAcquireNextImageKHR);
VK_DEFINE_FUNCTION(vkQueuePresentKHR);
VK_DEFINE_FUNCTION(vkGetSwapchainImagesKHR);
VK_DEFINE_FUNCTION(vkCreateDevice);
VK_DEFINE_FUNCTION(vkDestroyDevice);
VK_DEFINE_FUNCTION(vkDeviceWaitIdle);
VK_DEFINE_FUNCTION(vkEnumerateDeviceExtensionProperties);
VK_DEFINE_FUNCTION(vkGetDeviceQueue);
VK_DEFINE_FUNCTION(vkCreateCommandPool);
VK_DEFINE_FUNCTION(vkDestroyCommandPool);
VK_DEFINE_FUNCTION(vkTrimCommandPool);
VK_DEFINE_FUNCTION(vkResetCommandPool);
VK_DEFINE_FUNCTION(vkAllocateCommandBuffers);
VK_DEFINE_FUNCTION(vkFreeCommandBuffers);
VK_DEFINE_FUNCTION(vkResetCommandBuffer);
VK_DEFINE_FUNCTION(vkBeginCommandBuffer);
VK_DEFINE_FUNCTION(vkEndCommandBuffer);
VK_DEFINE_FUNCTION(vkQueueSubmit);
VK_DEFINE_FUNCTION(vkQueueWaitIdle);
VK_DEFINE_FUNCTION(vkCmdExecuteCommands);
VK_DEFINE_FUNCTION(vkCmdSetDeviceMask);
VK_DEFINE_FUNCTION(vkCreateFence);
VK_DEFINE_FUNCTION(vkDestroyFence);
VK_DEFINE_FUNCTION(vkGetFenceStatus);
VK_DEFINE_FUNCTION(vkResetFences);
VK_DEFINE_FUNCTION(vkWaitForFences);
VK_DEFINE_FUNCTION(vkCreateSemaphore);
VK_DEFINE_FUNCTION(vkDestroySemaphore);
VK_DEFINE_FUNCTION(vkCreateEvent);
VK_DEFINE_FUNCTION(vkDestroyEvent);
VK_DEFINE_FUNCTION(vkGetEventStatus);
VK_DEFINE_FUNCTION(vkSetEvent);
VK_DEFINE_FUNCTION(vkResetEvent);
VK_DEFINE_FUNCTION(vkCmdPipelineBarrier);
VK_DEFINE_FUNCTION(vkCmdClearColorImage);

#undef VK_DEFINE_FUNCTION

#define VK_BIND_FUNCTION(i__, name__) name__ = (PFN_##name__)vkGetInstanceProcAddr(i__, #name__)

void libvk_load_default_functions(PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr_)
{
    vkGetInstanceProcAddr = vkGetInstanceProcAddr_;

    VK_BIND_FUNCTION(NULL, vkCreateInstance);
    VK_BIND_FUNCTION(NULL, vkEnumerateInstanceVersion);
    VK_BIND_FUNCTION(NULL, vkEnumerateInstanceExtensionProperties);
    VK_BIND_FUNCTION(NULL, vkEnumerateInstanceLayerProperties);
}

void libvk_load_instance_functions(VkInstance vkInstance)
{
    VK_BIND_FUNCTION(vkInstance, vkDestroyInstance);
    VK_BIND_FUNCTION(vkInstance, vkCreateWin32SurfaceKHR);
    VK_BIND_FUNCTION(vkInstance, vkDestroySurfaceKHR);
    VK_BIND_FUNCTION(vkInstance, vkEnumeratePhysicalDevices);
    VK_BIND_FUNCTION(vkInstance, vkGetPhysicalDeviceProperties);
    VK_BIND_FUNCTION(vkInstance, vkGetPhysicalDeviceProperties2);
    VK_BIND_FUNCTION(vkInstance, vkGetPhysicalDeviceProperties2KHR);
    VK_BIND_FUNCTION(vkInstance, vkGetPhysicalDeviceQueueFamilyProperties);
    VK_BIND_FUNCTION(vkInstance, vkGetPhysicalDeviceSurfaceSupportKHR);
    VK_BIND_FUNCTION(vkInstance, vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
    VK_BIND_FUNCTION(vkInstance, vkGetPhysicalDeviceWin32PresentationSupportKHR);
    VK_BIND_FUNCTION(vkInstance, vkGetPhysicalDeviceSurfaceFormatsKHR);
    VK_BIND_FUNCTION(vkInstance, vkGetPhysicalDeviceSurfacePresentModesKHR);
    VK_BIND_FUNCTION(vkInstance, vkCreateSwapchainKHR);
    VK_BIND_FUNCTION(vkInstance, vkDestroySwapchainKHR);
    VK_BIND_FUNCTION(vkInstance, vkAcquireNextImageKHR);
    VK_BIND_FUNCTION(vkInstance, vkQueuePresentKHR);
    VK_BIND_FUNCTION(vkInstance, vkGetSwapchainImagesKHR);
    VK_BIND_FUNCTION(vkInstance, vkCreateDevice);
    VK_BIND_FUNCTION(vkInstance, vkDestroyDevice);
    VK_BIND_FUNCTION(vkInstance, vkDeviceWaitIdle);
    VK_BIND_FUNCTION(vkInstance, vkEnumerateDeviceExtensionProperties);
    VK_BIND_FUNCTION(vkInstance, vkGetDeviceQueue);
    VK_BIND_FUNCTION(vkInstance, vkCreateCommandPool);
    VK_BIND_FUNCTION(vkInstance, vkDestroyCommandPool);
    VK_BIND_FUNCTION(vkInstance, vkTrimCommandPool);
    VK_BIND_FUNCTION(vkInstance, vkResetCommandPool);
    VK_BIND_FUNCTION(vkInstance, vkAllocateCommandBuffers);
    VK_BIND_FUNCTION(vkInstance, vkFreeCommandBuffers);
    VK_BIND_FUNCTION(vkInstance, vkResetCommandBuffer);
    VK_BIND_FUNCTION(vkInstance, vkBeginCommandBuffer);
    VK_BIND_FUNCTION(vkInstance, vkEndCommandBuffer);
    VK_BIND_FUNCTION(vkInstance, vkQueueSubmit);
    VK_BIND_FUNCTION(vkInstance, vkQueueWaitIdle);
    VK_BIND_FUNCTION(vkInstance, vkCmdExecuteCommands);
    VK_BIND_FUNCTION(vkInstance, vkCmdSetDeviceMask);
    VK_BIND_FUNCTION(vkInstance, vkCreateFence);
    VK_BIND_FUNCTION(vkInstance, vkDestroyFence);
    VK_BIND_FUNCTION(vkInstance, vkGetFenceStatus);
    VK_BIND_FUNCTION(vkInstance, vkResetFences);
    VK_BIND_FUNCTION(vkInstance, vkWaitForFences);
    VK_BIND_FUNCTION(vkInstance, vkCreateSemaphore);
    VK_BIND_FUNCTION(vkInstance, vkDestroySemaphore);
    VK_BIND_FUNCTION(vkInstance, vkCreateEvent);
    VK_BIND_FUNCTION(vkInstance, vkDestroyEvent);
    VK_BIND_FUNCTION(vkInstance, vkGetEventStatus);
    VK_BIND_FUNCTION(vkInstance, vkSetEvent);
    VK_BIND_FUNCTION(vkInstance, vkResetEvent);
    VK_BIND_FUNCTION(vkInstance, vkCmdPipelineBarrier);
    VK_BIND_FUNCTION(vkInstance, vkCmdClearColorImage);
}
