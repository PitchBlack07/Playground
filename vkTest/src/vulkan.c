
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
DECLARE_VK_FUNCTION(vkCreateDevice);
DECLARE_VK_FUNCTION(vkDestroyDevice);
DECLARE_VK_FUNCTION(vkDeviceWaitIdle);
DECLARE_VK_FUNCTION(vkEnumerateDeviceExtensionProperties);
DECLARE_VK_FUNCTION(vkGetDeviceQueue);

void LoadVulkan()
{
	vkLibrary = LoadLibrary(TEXT("vulkan-1.dll"));
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
	VkInstance                                  instance,
	const char*                                 pName)
{
	return vkGetInstanceProcAddr__impl(instance, pName);
}


VKAPI_ATTR VkResult VKAPI_CALL vkCreateInstance(
	const VkInstanceCreateInfo*                 pCreateInfo,
	const VkAllocationCallbacks*                pAllocator,
	VkInstance*                                 pInstance)
{
	VkResult r = vkCreateInstance__impl(pCreateInfo, pAllocator, pInstance);
	if(r == VK_SUCCESS)
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
		LOAD_VK_FUNCTION_EX(*pInstance, vkCreateDevice);
		LOAD_VK_FUNCTION_EX(*pInstance, vkDestroyDevice);
		LOAD_VK_FUNCTION_EX(*pInstance, vkDeviceWaitIdle);
		LOAD_VK_FUNCTION_EX(*pInstance, vkEnumerateDeviceExtensionProperties);
		LOAD_VK_FUNCTION_EX(*pInstance, vkGetDeviceQueue);
	}

	return r;
}

VKAPI_ATTR void VKAPI_CALL vkDestroyInstance(
	VkInstance                                  instance,
	const VkAllocationCallbacks*                pAllocator)
{
	vkDestroyInstance__impl(instance, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceExtensionProperties(
	const char*                                 pLayerName,
	uint32_t*                                   pPropertyCount,
	VkExtensionProperties*                      pProperties)
{
	return vkEnumerateInstanceExtensionProperties__impl(pLayerName, pPropertyCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceVersion(
	uint32_t*                                   pApiVersion)
{
	return vkEnumerateInstanceVersion__impl(pApiVersion);
}

VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceLayerProperties(
	uint32_t*                                   pPropertyCount,
	VkLayerProperties*                          pProperties)
{
	return vkEnumerateInstanceLayerProperties__impl(pPropertyCount, pProperties);
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetDeviceProcAddr(
	VkDevice                                    device,
	const char*                                 pName)
{
	return vkGetDeviceProcAddr__impl(device, pName);
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceProperties(
	VkPhysicalDevice                            physicalDevice,
	VkPhysicalDeviceProperties*                 pProperties)
{
	vkGetPhysicalDeviceProperties__impl(physicalDevice, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateWin32SurfaceKHR(
	VkInstance                                  instance,
	const VkWin32SurfaceCreateInfoKHR*          pCreateInfo,
	const VkAllocationCallbacks*                pAllocator,
	VkSurfaceKHR*                               pSurface)
{
	return vkCreateWin32SurfaceKHR__impl(instance, pCreateInfo, pAllocator, pSurface);
}

VKAPI_ATTR void VKAPI_CALL vkDestroySurfaceKHR(
	VkInstance                                  instance,
	VkSurfaceKHR                                surface,
	const VkAllocationCallbacks*                pAllocator)
{
	vkDestroySurfaceKHR__impl(instance, surface, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkEnumeratePhysicalDevices(
	VkInstance                                  instance,
	uint32_t*                                   pPhysicalDeviceCount,
	VkPhysicalDevice*                           pPhysicalDevices)
{
	return vkEnumeratePhysicalDevices__impl(instance, pPhysicalDeviceCount, pPhysicalDevices);
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceProperties2(
	VkPhysicalDevice                            physicalDevice,
	VkPhysicalDeviceProperties2*                pProperties)
{
	vkGetPhysicalDeviceProperties2__impl(physicalDevice, pProperties);
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceProperties2KHR(
	VkPhysicalDevice                            physicalDevice,
	VkPhysicalDeviceProperties2*                pProperties)
{
	vkGetPhysicalDeviceProperties2KHR__impl(physicalDevice, pProperties);
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceQueueFamilyProperties(
	VkPhysicalDevice                            physicalDevice,
	uint32_t*                                   pQueueFamilyPropertyCount,
	VkQueueFamilyProperties*                    pQueueFamilyProperties)
{
	vkGetPhysicalDeviceQueueFamilyProperties__impl(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
}
VKAPI_ATTR VkResult VKAPI_CALL vkCreateDevice(
	VkPhysicalDevice                            physicalDevice,
	const VkDeviceCreateInfo*                   pCreateInfo,
	const VkAllocationCallbacks*                pAllocator,
	VkDevice*                                   pDevice)
{
	return vkCreateDevice__impl(physicalDevice, pCreateInfo, pAllocator, pDevice);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDevice(
	VkDevice                                    device,
	const VkAllocationCallbacks*                pAllocator)
{
	vkDestroyDevice__impl(device, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkDeviceWaitIdle(
	VkDevice                                    device)
{
	return vkDeviceWaitIdle__impl(device);
}

VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceExtensionProperties(
	VkPhysicalDevice                            physicalDevice,
	const char*                                 pLayerName,
	uint32_t*                                   pPropertyCount,
	VkExtensionProperties*                      pProperties)
{
	return vkEnumerateDeviceExtensionProperties__impl(physicalDevice, pLayerName, pPropertyCount, pProperties);
}

VKAPI_ATTR void VKAPI_CALL vkGetDeviceQueue(
	VkDevice                                    device,
	uint32_t                                    queueFamilyIndex,
	uint32_t                                    queueIndex,
	VkQueue*                                    pQueue)
{
	vkGetDeviceQueue__impl(device, queueFamilyIndex, queueIndex, pQueue);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceSupportKHR(
	VkPhysicalDevice                            physicalDevice,
	uint32_t                                    queueFamilyIndex,
	VkSurfaceKHR                                surface,
	VkBool32*                                   pSupported)
{
	return vkGetPhysicalDeviceSurfaceSupportKHR__impl(physicalDevice, queueFamilyIndex, surface, pSupported);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
	VkPhysicalDevice                            physicalDevice,
	VkSurfaceKHR                                surface,
	VkSurfaceCapabilitiesKHR*                   pSurfaceCapabilities)
{
	return vkGetPhysicalDeviceSurfaceCapabilitiesKHR__impl(physicalDevice, surface, pSurfaceCapabilities);
}

VKAPI_ATTR VkBool32 VKAPI_CALL vkGetPhysicalDeviceWin32PresentationSupportKHR(
	VkPhysicalDevice                            physicalDevice,
	uint32_t                                    queueFamilyIndex)
{
	return vkGetPhysicalDeviceWin32PresentationSupportKHR__impl(physicalDevice, queueFamilyIndex);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceFormatsKHR(
	VkPhysicalDevice                            physicalDevice,
	VkSurfaceKHR                                surface,
	uint32_t*                                   pSurfaceFormatCount,
	VkSurfaceFormatKHR*                         pSurfaceFormats)
{
	return vkGetPhysicalDeviceSurfaceFormatsKHR__impl(physicalDevice, surface, pSurfaceFormatCount, pSurfaceFormats);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfacePresentModesKHR(
	VkPhysicalDevice                            physicalDevice,
	VkSurfaceKHR                                surface,
	uint32_t*                                   pPresentModeCount,
	VkPresentModeKHR*                           pPresentModes)
{
	return vkGetPhysicalDeviceSurfacePresentModesKHR__impl(physicalDevice, surface, pPresentModeCount, pPresentModes);
}
