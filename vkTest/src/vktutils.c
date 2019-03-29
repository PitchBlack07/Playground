
#include <vktutils.h>
#include <vktMemory.h>
#include <vulkan.h>
#include <stdio.h>
#include <string.h>
uint32_t SelectSupportedExtensions(const char* const* requested_, const char** supported_, uint32_t size_)
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

uint32_t SelectSupportedLayers(const char* const* requested_, const char** supported_, uint32_t size_) 
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