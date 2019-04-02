
#include <vktutils.h>
#include <vktMemory.h>
#include <vulkan.h>
#include <stdio.h>
#include <string.h>
uint32_t vktSelectSupportedExtensions(const char* const* requested_, const char** supported_, uint32_t size_)
{
	uint32_t k = 0;
	uint32_t count = 0;
	vkEnumerateInstanceExtensionProperties(NULL, &count, NULL);
	if (count)
	{
		uint32_t i, j;
		VkExtensionProperties* properties = ALLOC_POD_ARRAY(VkExtensionProperties, count);
		vkEnumerateInstanceExtensionProperties(NULL, &count, properties);

		for (j = 0; j < size_; ++j) {
			for (i = 0; i < count; ++i) {
				if (strcmp(requested_[j], properties[i].extensionName) == 0) {
					supported_[k++] = requested_[j];
					printf("Activated Extension %0u: %s\n", k - 1, properties[i].extensionName);
					break;
				}
			}
		}

		FREE_MEMORY(properties);
	}
	return k;
}

uint32_t vktSelectSupportedLayers(const char* const* requested_, const char** supported_, uint32_t size_)
{
	uint32_t k = 0;
	uint32_t count = 0;
	vkEnumerateInstanceLayerProperties(&count, NULL);
	if (count)
	{
		uint32_t i, j;
		VkLayerProperties* properties = ALLOC_POD_ARRAY(VkLayerProperties, count);
		vkEnumerateInstanceLayerProperties(&count, properties);

		for (j = 0; j < size_; ++j) {
			for (i = 0; i < count; ++i) {
				if (strcmp(requested_[j], properties[i].layerName) == 0) {
					supported_[k++] = requested_[j];
					printf("Activated Layer %0u: %s\n", k - 1, properties[i].layerName);
					break;
				}
			}
		}

		FREE_MEMORY(properties);
	}
	return k;
}

void vkPrintAvaiableExtensions()
{
	uint32_t count = 0;
	vkEnumerateInstanceExtensionProperties(NULL, &count, NULL);
	if (count)
	{
		uint32_t i;
		VkExtensionProperties* properties = ALLOC_POD_ARRAY(VkExtensionProperties, count);
		vkEnumerateInstanceExtensionProperties(NULL, &count, properties);

		for (i = 0; i < count; ++i) {
			printf("vkExtension %0u: %s\n", i, properties[i].extensionName);
		}

		FREE_MEMORY(properties);
	}
}

void vktPrintAvailableLayers()
{
	uint32_t count = 0;
	vkEnumerateInstanceLayerProperties(&count, NULL);
	if (count) {

		VkLayerProperties* const properties = ALLOC_POD_ARRAY(VkLayerProperties, count);
		if (properties) {
			uint32_t i = 0;
			vkEnumerateInstanceLayerProperties(&count, properties);

			for (i = 0; i < count; ++i) {
				printf("vkLayer %0u: %s\n", i, properties[i].layerName);
			}
		}
		FREE_MEMORY(properties);
	}
}

VktVersion vktGetVersion()
{
	uint32_t api = 0;
	vkEnumerateInstanceVersion(&api);

	VktVersion v = {
		.Major = VK_VERSION_MAJOR(api),
		.Minor = VK_VERSION_MINOR(api),
		.Build = VK_VERSION_PATCH(api)
	};

	return v;
}

uint32_t vktGetGraphicsQueueIndex(const VktQueueInfo* info_)
{
	if (info_) {
		for (uint32_t i = 0; i < info_->Count; ++i) {
			if (info_->Properties[i].queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT)) {
				return i;
			}
		}
	}
	return -1;
}

void vktPrintDeviceExtensions(VkPhysicalDevice device_, const char* layer_)
{
	uint32_t i = 0;
	uint32_t count = 0;
	vkEnumerateDeviceExtensionProperties(device_, layer_, &count, NULL);
	if (count) {
		VkExtensionProperties* properties = ALLOC_POD_ARRAY(VkExtensionProperties, count);
		vkEnumerateDeviceExtensionProperties(device_, layer_, &count, properties);

		for (i = 0; i < count; ++i) {
			printf("vkDeviceExtension %0u: %s\n", i, properties[i].extensionName);
		}

		FREE_MEMORY(properties);
	}
}

VktDeviceExtensions vktGetDeviceExtensions(VkPhysicalDevice device_, const char* layer_)
{
	uint32_t i = 0, count = 0;
	VktDeviceExtensions vtkExtensions = { 0 };

	vkEnumerateDeviceExtensionProperties(device_, layer_, &count, NULL);
	if (count) {
		VkExtensionProperties* const extensions = ALLOC_POD_ARRAY(VkExtensionProperties, count);
		vkEnumerateDeviceExtensionProperties(device_, layer_, &count, extensions);

		if (extensions) {
			vtkExtensions.Count = count;
			vtkExtensions.Extensions = extensions;
		}
	}

	return vtkExtensions;
}

void vktFreeDeviceExtensions(VktDeviceExtensions* extensions_)
{
	if (extensions_)
	{
		FREE_MEMORY((void*)extensions_->Extensions);
		extensions_->Count = 0;
		extensions_->Extensions = NULL;
	}
}

VktQueueInfo vktGetQueueInfos(VkPhysicalDevice device_)
{
	uint32_t count = 0, i = 0;
	VktQueueInfo info = { 0 };

	vkGetPhysicalDeviceQueueFamilyProperties(device_, &count, NULL);

	if (count) {
		VkQueueFamilyProperties* const queueFamilyProperties = ALLOC_POD_ARRAY(VkQueueFamilyProperties, count);
		if (queueFamilyProperties) {
			vkGetPhysicalDeviceQueueFamilyProperties(device_, &count, queueFamilyProperties);

			info.Count = count;
			info.Properties = queueFamilyProperties;
		}
	}

	return info;
}
void vktFreeQueueInfo(VktQueueInfo* info_)
{
	if (info_) {
		FREE_MEMORY((void*)info_->Properties);
		info_->Count = 0;
		info_->Properties = 0;
	}
}

uint32_t vktGetPresentationQueueIndex(const VktQueueInfo* info_, VkPhysicalDevice device_, VkSurfaceKHR surface_) {

	uint32_t i = 0;

	for (i = 0; i < info_->Count; ++i) {

		VkBool32 supported = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(device_, i, surface_, &supported);

		if (supported == VK_TRUE) {
			return i;
		}
	}

	return -1;
}

VktPresentationInfo vktGetPresentationInfo(VkPhysicalDevice device_, VkSurfaceKHR surface_)
{
	uint32_t i = 0, count = 0;
	VktPresentationInfo retVal = { 0 };

	vkGetPhysicalDeviceSurfaceFormatsKHR(device_, surface_, &count, NULL);

	VkSurfaceFormatKHR* formats = ALLOC_POD_ARRAY(VkSurfaceFormatKHR, count);
	if (formats) {
		vkGetPhysicalDeviceSurfaceFormatsKHR(device_, surface_, &count, formats);

		retVal.formats.Count   = count;
		retVal.formats.Formats = formats;
	}

	vkGetPhysicalDeviceSurfacePresentModesKHR(device_, surface_, &count, NULL);
	VkPresentModeKHR* modes = ALLOC_POD_ARRAY(VkPresentModeKHR, count);
	if (modes) {
		vkGetPhysicalDeviceSurfacePresentModesKHR(device_, surface_, &count, modes);

		retVal.modes.Values = modes;
		retVal.modes.Count  = count;
	}

	return retVal;
}

void vktFreePresentationInfo(VktPresentationInfo* formats_)
{
	FREE_MEMORY((void*)formats_->formats.Formats);
	FREE_MEMORY((void*)formats_->modes.Values);

	memset(formats_, 0, sizeof(*formats_));
}