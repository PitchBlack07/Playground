
#include <vulkan.h>
#include <Windows.h>
#define LOAD_VK_FUNCTION(name__) name__##__impl = (PFN_##name__)vkGetInstanceProcAddr(NULL, #name__)
#define LOAD_VK_FUNCTION_EX(i__, name__) name__##__impl = (PFN_##name__)vkGetInstanceProcAddr(i__, #name__)
#define DECLARE_VK_FUNCTION(name__) PFN_##name__ name__##__impl = NULL

HMODULE vkLibrary = NULL;

DECLARE_VK_FUNCTION(vkGetInstanceProcAddr);
DECLARE_VK_FUNCTION(vkCreateInstance);
DECLARE_VK_FUNCTION(vkDestroyInstance);
DECLARE_VK_FUNCTION(vkEnumerateInstanceVersion);
DECLARE_VK_FUNCTION(vkEnumerateInstanceExtensionProperties);
DECLARE_VK_FUNCTION(vkEnumerateInstanceLayerProperties);
DECLARE_VK_FUNCTION(vkGetDeviceProcAddr);
DECLARE_VK_FUNCTION(vkGetPhysicalDeviceProperties);
DECLARE_VK_FUNCTION(vkCreateWin32SurfaceKHR);
DECLARE_VK_FUNCTION(vkDestroySurfaceKHR);
DECLARE_VK_FUNCTION(vkEnumeratePhysicalDevices);
DECLARE_VK_FUNCTION(vkGetPhysicalDeviceProperties2);
DECLARE_VK_FUNCTION(vkGetPhysicalDeviceProperties2KHR);
DECLARE_VK_FUNCTION(vkGetPhysicalDeviceQueueFamilyProperties);
DECLARE_VK_FUNCTION(vkGetPhysicalDeviceSurfaceSupportKHR);
DECLARE_VK_FUNCTION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
DECLARE_VK_FUNCTION(vkGetPhysicalDeviceWin32PresentationSupportKHR);
DECLARE_VK_FUNCTION(vkGetPhysicalDeviceSurfaceFormatsKHR);
DECLARE_VK_FUNCTION(vkGetPhysicalDeviceSurfacePresentModesKHR);
DECLARE_VK_FUNCTION(vkCreateSwapchainKHR);
DECLARE_VK_FUNCTION(vkDestroySwapchainKHR);
DECLARE_VK_FUNCTION(vkAcquireNextImageKHR);
DECLARE_VK_FUNCTION(vkQueuePresentKHR);
DECLARE_VK_FUNCTION(vkGetSwapchainImagesKHR);
DECLARE_VK_FUNCTION(vkCreateDevice);
DECLARE_VK_FUNCTION(vkDestroyDevice);
DECLARE_VK_FUNCTION(vkDeviceWaitIdle);
DECLARE_VK_FUNCTION(vkEnumerateDeviceExtensionProperties);
DECLARE_VK_FUNCTION(vkGetDeviceQueue);
DECLARE_VK_FUNCTION(vkCreateCommandPool);
DECLARE_VK_FUNCTION(vkDestroyCommandPool);
DECLARE_VK_FUNCTION(vkTrimCommandPool);
DECLARE_VK_FUNCTION(vkResetCommandPool);
DECLARE_VK_FUNCTION(vkAllocateCommandBuffers);
DECLARE_VK_FUNCTION(vkFreeCommandBuffers);
DECLARE_VK_FUNCTION(vkResetCommandBuffer);
DECLARE_VK_FUNCTION(vkBeginCommandBuffer);
DECLARE_VK_FUNCTION(vkEndCommandBuffer);
DECLARE_VK_FUNCTION(vkQueueSubmit);
DECLARE_VK_FUNCTION(vkQueueWaitIdle);
DECLARE_VK_FUNCTION(vkCmdExecuteCommands);
DECLARE_VK_FUNCTION(vkCmdSetDeviceMask);

DECLARE_VK_FUNCTION(vkCreateFence);
DECLARE_VK_FUNCTION(vkDestroyFence);
DECLARE_VK_FUNCTION(vkGetFenceStatus);
DECLARE_VK_FUNCTION(vkResetFences);
DECLARE_VK_FUNCTION(vkWaitForFences);

DECLARE_VK_FUNCTION(vkCreateSemaphore);
DECLARE_VK_FUNCTION(vkDestroySemaphore);

DECLARE_VK_FUNCTION(vkCreateEvent);
DECLARE_VK_FUNCTION(vkDestroyEvent);
DECLARE_VK_FUNCTION(vkGetEventStatus);
DECLARE_VK_FUNCTION(vkSetEvent);
DECLARE_VK_FUNCTION(vkResetEvent);

DECLARE_VK_FUNCTION(vkCmdClearColorImage);

DECLARE_VK_FUNCTION(vkCmdPipelineBarrier);

void LoadVulkan()
{
    vkLibrary                   = LoadLibrary(TEXT("vulkan-1.dll"));
    vkGetInstanceProcAddr__impl = (PFN_vkGetInstanceProcAddr)GetProcAddress(vkLibrary, "vkGetInstanceProcAddr");

    LOAD_VK_FUNCTION(vkCreateInstance);

    LOAD_VK_FUNCTION(vkEnumerateInstanceVersion);
    LOAD_VK_FUNCTION(vkEnumerateInstanceExtensionProperties);
    LOAD_VK_FUNCTION(vkEnumerateInstanceLayerProperties);
    LOAD_VK_FUNCTION(vkGetDeviceProcAddr);
}

void UnloadVulkan()
{
    FreeLibrary(vkLibrary);
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetInstanceProcAddr(
    VkInstance  instance,
    const char* pName)
{
    return vkGetInstanceProcAddr__impl(instance, pName);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateInstance(
    const VkInstanceCreateInfo*  pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkInstance*                  pInstance)
{
    VkResult r = vkCreateInstance__impl(pCreateInfo, pAllocator, pInstance);
    if (r == VK_SUCCESS)
    {
        LOAD_VK_FUNCTION_EX(*pInstance, vkDestroyInstance);
        LOAD_VK_FUNCTION_EX(*pInstance, vkCreateWin32SurfaceKHR);
        LOAD_VK_FUNCTION_EX(*pInstance, vkDestroySurfaceKHR);
        LOAD_VK_FUNCTION_EX(*pInstance, vkEnumeratePhysicalDevices);
        LOAD_VK_FUNCTION_EX(*pInstance, vkGetPhysicalDeviceProperties);
        LOAD_VK_FUNCTION_EX(*pInstance, vkGetPhysicalDeviceProperties2);
        LOAD_VK_FUNCTION_EX(*pInstance, vkGetPhysicalDeviceProperties2KHR);
        LOAD_VK_FUNCTION_EX(*pInstance, vkGetPhysicalDeviceQueueFamilyProperties);
        LOAD_VK_FUNCTION_EX(*pInstance, vkGetPhysicalDeviceSurfaceSupportKHR);
        LOAD_VK_FUNCTION_EX(*pInstance, vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
        LOAD_VK_FUNCTION_EX(*pInstance, vkGetPhysicalDeviceWin32PresentationSupportKHR);
        LOAD_VK_FUNCTION_EX(*pInstance, vkGetPhysicalDeviceSurfaceFormatsKHR);
        LOAD_VK_FUNCTION_EX(*pInstance, vkGetPhysicalDeviceSurfacePresentModesKHR);
        LOAD_VK_FUNCTION_EX(*pInstance, vkCreateSwapchainKHR);
        LOAD_VK_FUNCTION_EX(*pInstance, vkDestroySwapchainKHR);
        LOAD_VK_FUNCTION_EX(*pInstance, vkAcquireNextImageKHR);
        LOAD_VK_FUNCTION_EX(*pInstance, vkQueuePresentKHR);
        LOAD_VK_FUNCTION_EX(*pInstance, vkGetSwapchainImagesKHR);
        LOAD_VK_FUNCTION_EX(*pInstance, vkCreateDevice);
        LOAD_VK_FUNCTION_EX(*pInstance, vkDestroyDevice);
        LOAD_VK_FUNCTION_EX(*pInstance, vkDeviceWaitIdle);
        LOAD_VK_FUNCTION_EX(*pInstance, vkEnumerateDeviceExtensionProperties);
        LOAD_VK_FUNCTION_EX(*pInstance, vkGetDeviceQueue);
        LOAD_VK_FUNCTION_EX(*pInstance, vkCreateCommandPool);
        LOAD_VK_FUNCTION_EX(*pInstance, vkDestroyCommandPool);
        LOAD_VK_FUNCTION_EX(*pInstance, vkTrimCommandPool);
        LOAD_VK_FUNCTION_EX(*pInstance, vkResetCommandPool);
        LOAD_VK_FUNCTION_EX(*pInstance, vkAllocateCommandBuffers);
        LOAD_VK_FUNCTION_EX(*pInstance, vkFreeCommandBuffers);
        LOAD_VK_FUNCTION_EX(*pInstance, vkResetCommandBuffer);
        LOAD_VK_FUNCTION_EX(*pInstance, vkBeginCommandBuffer);
        LOAD_VK_FUNCTION_EX(*pInstance, vkEndCommandBuffer);
        LOAD_VK_FUNCTION_EX(*pInstance, vkQueueSubmit);
        LOAD_VK_FUNCTION_EX(*pInstance, vkQueueWaitIdle);
        LOAD_VK_FUNCTION_EX(*pInstance, vkCmdExecuteCommands);
        LOAD_VK_FUNCTION_EX(*pInstance, vkCmdSetDeviceMask);

		LOAD_VK_FUNCTION_EX(*pInstance, vkCreateFence);
        LOAD_VK_FUNCTION_EX(*pInstance, vkDestroyFence);
        LOAD_VK_FUNCTION_EX(*pInstance, vkGetFenceStatus);
        LOAD_VK_FUNCTION_EX(*pInstance, vkResetFences);
        LOAD_VK_FUNCTION_EX(*pInstance, vkWaitForFences);

		LOAD_VK_FUNCTION_EX(*pInstance, vkCreateSemaphore);
        LOAD_VK_FUNCTION_EX(*pInstance, vkDestroySemaphore);

		LOAD_VK_FUNCTION_EX(*pInstance, vkCreateEvent);
        LOAD_VK_FUNCTION_EX(*pInstance, vkDestroyEvent);
        LOAD_VK_FUNCTION_EX(*pInstance, vkGetEventStatus);
        LOAD_VK_FUNCTION_EX(*pInstance, vkSetEvent);
        LOAD_VK_FUNCTION_EX(*pInstance, vkResetEvent);
        LOAD_VK_FUNCTION_EX(*pInstance, vkCmdPipelineBarrier);

		LOAD_VK_FUNCTION_EX(*pInstance, vkCmdClearColorImage);
    }

    return r;
}

VKAPI_ATTR void VKAPI_CALL vkDestroyInstance(
    VkInstance                   instance,
    const VkAllocationCallbacks* pAllocator)
{
    vkDestroyInstance__impl(instance, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceExtensionProperties(
    const char*            pLayerName,
    uint32_t*              pPropertyCount,
    VkExtensionProperties* pProperties)
{
    return vkEnumerateInstanceExtensionProperties__impl(pLayerName, pPropertyCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceVersion(
    uint32_t* pApiVersion)
{
    return vkEnumerateInstanceVersion__impl(pApiVersion);
}

VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceLayerProperties(
    uint32_t*          pPropertyCount,
    VkLayerProperties* pProperties)
{
    return vkEnumerateInstanceLayerProperties__impl(pPropertyCount, pProperties);
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetDeviceProcAddr(
    VkDevice    device,
    const char* pName)
{
    return vkGetDeviceProcAddr__impl(device, pName);
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceProperties(
    VkPhysicalDevice            physicalDevice,
    VkPhysicalDeviceProperties* pProperties)
{
    vkGetPhysicalDeviceProperties__impl(physicalDevice, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateWin32SurfaceKHR(
    VkInstance                         instance,
    const VkWin32SurfaceCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks*       pAllocator,
    VkSurfaceKHR*                      pSurface)
{
    return vkCreateWin32SurfaceKHR__impl(instance, pCreateInfo, pAllocator, pSurface);
}

VKAPI_ATTR void VKAPI_CALL vkDestroySurfaceKHR(
    VkInstance                   instance,
    VkSurfaceKHR                 surface,
    const VkAllocationCallbacks* pAllocator)
{
    vkDestroySurfaceKHR__impl(instance, surface, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkEnumeratePhysicalDevices(
    VkInstance        instance,
    uint32_t*         pPhysicalDeviceCount,
    VkPhysicalDevice* pPhysicalDevices)
{
    return vkEnumeratePhysicalDevices__impl(instance, pPhysicalDeviceCount, pPhysicalDevices);
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceProperties2(
    VkPhysicalDevice             physicalDevice,
    VkPhysicalDeviceProperties2* pProperties)
{
    vkGetPhysicalDeviceProperties2__impl(physicalDevice, pProperties);
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceProperties2KHR(
    VkPhysicalDevice             physicalDevice,
    VkPhysicalDeviceProperties2* pProperties)
{
    vkGetPhysicalDeviceProperties2KHR__impl(physicalDevice, pProperties);
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceQueueFamilyProperties(
    VkPhysicalDevice         physicalDevice,
    uint32_t*                pQueueFamilyPropertyCount,
    VkQueueFamilyProperties* pQueueFamilyProperties)
{
    vkGetPhysicalDeviceQueueFamilyProperties__impl(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
}
VKAPI_ATTR VkResult VKAPI_CALL vkCreateDevice(
    VkPhysicalDevice             physicalDevice,
    const VkDeviceCreateInfo*    pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDevice*                    pDevice)
{
    return vkCreateDevice__impl(physicalDevice, pCreateInfo, pAllocator, pDevice);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDevice(
    VkDevice                     device,
    const VkAllocationCallbacks* pAllocator)
{
    vkDestroyDevice__impl(device, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkDeviceWaitIdle(
    VkDevice device)
{
    return vkDeviceWaitIdle__impl(device);
}

VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceExtensionProperties(
    VkPhysicalDevice       physicalDevice,
    const char*            pLayerName,
    uint32_t*              pPropertyCount,
    VkExtensionProperties* pProperties)
{
    return vkEnumerateDeviceExtensionProperties__impl(physicalDevice, pLayerName, pPropertyCount, pProperties);
}

VKAPI_ATTR void VKAPI_CALL vkGetDeviceQueue(
    VkDevice device,
    uint32_t queueFamilyIndex,
    uint32_t queueIndex,
    VkQueue* pQueue)
{
    vkGetDeviceQueue__impl(device, queueFamilyIndex, queueIndex, pQueue);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceSupportKHR(
    VkPhysicalDevice physicalDevice,
    uint32_t         queueFamilyIndex,
    VkSurfaceKHR     surface,
    VkBool32*        pSupported)
{
    return vkGetPhysicalDeviceSurfaceSupportKHR__impl(physicalDevice, queueFamilyIndex, surface, pSupported);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
    VkPhysicalDevice          physicalDevice,
    VkSurfaceKHR              surface,
    VkSurfaceCapabilitiesKHR* pSurfaceCapabilities)
{
    return vkGetPhysicalDeviceSurfaceCapabilitiesKHR__impl(physicalDevice, surface, pSurfaceCapabilities);
}

VKAPI_ATTR VkBool32 VKAPI_CALL vkGetPhysicalDeviceWin32PresentationSupportKHR(
    VkPhysicalDevice physicalDevice,
    uint32_t         queueFamilyIndex)
{
    return vkGetPhysicalDeviceWin32PresentationSupportKHR__impl(physicalDevice, queueFamilyIndex);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceFormatsKHR(
    VkPhysicalDevice    physicalDevice,
    VkSurfaceKHR        surface,
    uint32_t*           pSurfaceFormatCount,
    VkSurfaceFormatKHR* pSurfaceFormats)
{
    return vkGetPhysicalDeviceSurfaceFormatsKHR__impl(physicalDevice, surface, pSurfaceFormatCount, pSurfaceFormats);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfacePresentModesKHR(
    VkPhysicalDevice  physicalDevice,
    VkSurfaceKHR      surface,
    uint32_t*         pPresentModeCount,
    VkPresentModeKHR* pPresentModes)
{
    return vkGetPhysicalDeviceSurfacePresentModesKHR__impl(physicalDevice, surface, pPresentModeCount, pPresentModes);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateSwapchainKHR(
    VkDevice                        device,
    const VkSwapchainCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks*    pAllocator,
    VkSwapchainKHR*                 pSwapchain)
{
    return vkCreateSwapchainKHR__impl(device, pCreateInfo, pAllocator, pSwapchain);
}

VKAPI_ATTR void VKAPI_CALL vkDestroySwapchainKHR(
    VkDevice                     device,
    VkSwapchainKHR               swapchain,
    const VkAllocationCallbacks* pAllocator)
{
    vkDestroySwapchainKHR__impl(device, swapchain, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkAcquireNextImageKHR(
    VkDevice       device,
    VkSwapchainKHR swapchain,
    uint64_t       timeout,
    VkSemaphore    semaphore,
    VkFence        fence,
    uint32_t*      pImageIndex)
{
    return vkAcquireNextImageKHR__impl(device, swapchain, timeout, semaphore, fence, pImageIndex);
}

VKAPI_ATTR VkResult VKAPI_CALL vkQueuePresentKHR(
    VkQueue                 queue,
    const VkPresentInfoKHR* pPresentInfo)
{
    return vkQueuePresentKHR__impl(queue, pPresentInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetSwapchainImagesKHR(
    VkDevice       device,
    VkSwapchainKHR swapchain,
    uint32_t*      pSwapchainImageCount,
    VkImage*       pSwapchainImages)
{
    return vkGetSwapchainImagesKHR__impl(device, swapchain, pSwapchainImageCount, pSwapchainImages);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateCommandPool(
    VkDevice                       device,
    const VkCommandPoolCreateInfo* pCreateInfo,
    const VkAllocationCallbacks*   pAllocator,
    VkCommandPool*                 pCommandPool)
{
    return vkCreateCommandPool__impl(device, pCreateInfo, pAllocator, pCommandPool);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyCommandPool(
    VkDevice                     device,
    VkCommandPool                commandPool,
    const VkAllocationCallbacks* pAllocator)
{
    vkDestroyCommandPool__impl(device, commandPool, pAllocator);
}

VKAPI_ATTR void VKAPI_CALL vkTrimCommandPool(
	VkDevice               device,
	VkCommandPool          commandPool,
	VkCommandPoolTrimFlags flags)
{
    vkTrimCommandPool__impl(device, commandPool, flags);
}

VKAPI_ATTR VkResult VKAPI_CALL vkResetCommandPool(
	VkDevice                device,
	VkCommandPool           commandPool,
	VkCommandPoolResetFlags flags)
{
    return vkResetCommandPool__impl(device, commandPool, flags);
}

VKAPI_ATTR VkResult VKAPI_CALL vkAllocateCommandBuffers(
	VkDevice                           device,
	const VkCommandBufferAllocateInfo* pAllocateInfo,
	VkCommandBuffer*                   pCommandBuffers)
{
    return vkAllocateCommandBuffers__impl(device, pAllocateInfo, pCommandBuffers);
}

VKAPI_ATTR void VKAPI_CALL vkFreeCommandBuffers(
	VkDevice               device,
	VkCommandPool          commandPool,
	uint32_t               commandBufferCount,
	const VkCommandBuffer* pCommandBuffers)
{
    vkFreeCommandBuffers__impl(device, commandPool, commandBufferCount, pCommandBuffers);
}

VKAPI_ATTR VkResult VKAPI_CALL vkBeginCommandBuffer(
	VkCommandBuffer                 commandBuffer,
	const VkCommandBufferBeginInfo* pBeginInfo)
{
    return vkBeginCommandBuffer__impl(commandBuffer, pBeginInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL vkEndCommandBuffer(
	VkCommandBuffer commandBuffer)
{
    return vkEndCommandBuffer__impl(commandBuffer);
}

VKAPI_ATTR VkResult VKAPI_CALL vkResetCommandBuffer(
	VkCommandBuffer           commandBuffer,
	VkCommandBufferResetFlags flags)
{
    return vkResetCommandBuffer__impl(commandBuffer, flags);
}

VKAPI_ATTR VkResult VKAPI_CALL vkQueueSubmit(
	VkQueue             queue,
	uint32_t            submitCount,
	const VkSubmitInfo* pSubmits,
	VkFence             fence)
{
    return vkQueueSubmit__impl(queue, submitCount, pSubmits, fence);
}

VKAPI_ATTR VkResult VKAPI_CALL vkQueueWaitIdle(
	VkQueue queue)
{
    return vkQueueWaitIdle__impl(queue);
}

VKAPI_ATTR void VKAPI_CALL vkCmdExecuteCommands(
	VkCommandBuffer        commandBuffer,
	uint32_t               commandBufferCount,
	const VkCommandBuffer* pCommandBuffers)
{
    vkCmdExecuteCommands__impl(commandBuffer, commandBufferCount, pCommandBuffers);
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetDeviceMask(
	VkCommandBuffer commandBuffer,
	uint32_t        deviceMask)
{
    vkCmdSetDeviceMask__impl(commandBuffer, deviceMask);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateFence(
	VkDevice                     device,
	const VkFenceCreateInfo*     pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkFence*                     pFence)
{
    return vkCreateFence__impl(device, pCreateInfo, pAllocator, pFence);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyFence(
	VkDevice                     device,
	VkFence                      fence,
	const VkAllocationCallbacks* pAllocator)
{
    vkDestroyFence__impl(device, fence, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkResetFences(
	VkDevice       device,
	uint32_t       fenceCount,
	const VkFence* pFences)
{
    return vkResetFences__impl(device, fenceCount, pFences);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetFenceStatus(
	VkDevice device,
	VkFence  fence)
{
    return vkGetFenceStatus__impl(device, fence);
}

VKAPI_ATTR VkResult VKAPI_CALL vkWaitForFences(
	VkDevice       device,
	uint32_t       fenceCount,
	const VkFence* pFences,
	VkBool32       waitAll,
	uint64_t       timeout)
{
    return vkWaitForFences__impl(device, fenceCount, pFences, waitAll, timeout);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateSemaphore(
	VkDevice                     device,
	const VkSemaphoreCreateInfo* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkSemaphore*                 pSemaphore)
{
    return vkCreateSemaphore__impl(device, pCreateInfo, pAllocator, pSemaphore);
}

VKAPI_ATTR void VKAPI_CALL vkDestroySemaphore(
	VkDevice                     device,
	VkSemaphore                  semaphore,
	const VkAllocationCallbacks* pAllocator)
{
    vkDestroySemaphore__impl(device, semaphore, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateEvent(
	VkDevice                     device,
	const VkEventCreateInfo*     pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkEvent*                     pEvent)
{
    return vkCreateEvent__impl(device, pCreateInfo, pAllocator, pEvent);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyEvent(
	VkDevice                     device,
	VkEvent                      event,
	const VkAllocationCallbacks* pAllocator)
{
    vkDestroyEvent__impl(device, event, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetEventStatus(
	VkDevice device,
	VkEvent  event)
{
    return vkGetEventStatus__impl(device, event);
}

VKAPI_ATTR VkResult VKAPI_CALL vkSetEvent(
	VkDevice device,
	VkEvent  event)
{
    return vkSetEvent__impl(device, event);
}

VKAPI_ATTR VkResult VKAPI_CALL vkResetEvent(
	VkDevice device,
	VkEvent  event)
{
    return vkResetEvent__impl(device, event);
}

VKAPI_ATTR void VKAPI_CALL vkCmdClearColorImage(
	VkCommandBuffer                commandBuffer,
	VkImage                        image,
	VkImageLayout                  imageLayout,
	const VkClearColorValue*       pColor,
	uint32_t                       rangeCount,
	const VkImageSubresourceRange* pRanges)
{
    vkCmdClearColorImage__impl(commandBuffer, image, imageLayout, pColor, rangeCount, pRanges);
}

VKAPI_ATTR void VKAPI_CALL vkCmdPipelineBarrier(
	VkCommandBuffer              commandBuffer,
	VkPipelineStageFlags         srcStageMask,
	VkPipelineStageFlags         dstStageMask,
	VkDependencyFlags            dependencyFlags,
	uint32_t                     memoryBarrierCount,
	const VkMemoryBarrier*       pMemoryBarriers,
	uint32_t                     bufferMemoryBarrierCount,
	const VkBufferMemoryBarrier* pBufferMemoryBarriers,
	uint32_t                     imageMemoryBarrierCount,
	const VkImageMemoryBarrier*  pImageMemoryBarriers)
{
    vkCmdPipelineBarrier__impl(commandBuffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
}
