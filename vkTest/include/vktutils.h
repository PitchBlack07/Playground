#pragma once
#include <stdint.h>
#include <libvk.h>

typedef struct VktVersion_ {
	uint8_t  Major;
	uint8_t  Minor;
	uint16_t Build;
} VktVersion;


typedef struct VktDeviceExtensions_ {
	const VkExtensionProperties* Extensions;
	uint32_t                     Count;
} VktDeviceExtensions;

typedef struct VktQueueInfo_
{
	const VkQueueFamilyProperties* Properties;
	uint32_t                       Count;
} VktQueueInfo;

typedef struct VktPresentationInfo_
{
	struct
	{
		const VkSurfaceFormatKHR* Formats;
		uint32_t                  Count;
	} formats;

	struct
	{
		const VkPresentModeKHR* Values;
		uint32_t                Count;
	} modes;
} VktPresentationInfo;

VktVersion vktGetVersion();

uint32_t vktSelectSupportedExtensions(const char* const* requrested_, const char** supported_, uint32_t size_);
uint32_t vktSelectSupportedLayers(const char* const* requested_, const char** supported_, uint32_t size_);

VktDeviceExtensions vktGetDeviceExtensions(VkPhysicalDevice device_, const char* layer_);
VktQueueInfo vktGetQueueInfos(VkPhysicalDevice device_);
VktPresentationInfo vktGetPresentationInfo(VkPhysicalDevice device_, VkSurfaceKHR surface_);

void vktFreeDeviceExtensions(VktDeviceExtensions* extensions_);
void vktFreeQueueInfo(VktQueueInfo* info_);
void vktFreePresentationInfo(VktPresentationInfo* formats_);

uint32_t vktGetGraphicsQueueIndex(const VktQueueInfo* info_);
uint32_t vktGetPresentationQueueIndex(const VktQueueInfo* info_, VkPhysicalDevice device_, VkSurfaceKHR surface_);

void vktPrintAvailableLayers();
void vkPrintAvaiableExtensions();
void vktPrintDeviceExtensions(VkPhysicalDevice device_, const char* layer_);