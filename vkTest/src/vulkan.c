
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

void LoadVulkan()
{
	vkLibrary = LoadLibrary(TEXT("vulkan-1.dll"));
	vkGetInstanceProcAddr__impl = (PFN_vkGetInstanceProcAddr)GetProcAddress(vkLibrary, "vkGetInstanceProcAddr");

	LOAD_VK_FUNCTION(vkCreateInstance);
	
	LOAD_VK_FUNCTION(vkEnumerateInstanceVersion);
	LOAD_VK_FUNCTION(vkEnumerateInstanceExtensionProperties);
	LOAD_VK_FUNCTION(vkEnumerateInstanceLayerProperties);
	LOAD_VK_FUNCTION(vkGetDeviceProcAddr);
	LOAD_VK_FUNCTION(vkGetPhysicalDeviceProperties);
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

