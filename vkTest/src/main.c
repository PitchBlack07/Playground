#include <stdint.h>
#include <vulkan.h>
#include <Windows.h>
#include <stdio.h>
#include <vktMemory.h>
#include <vktutils.h>
#include <vktwindow.h>

extern void LoadVulkan();
extern void UnloadVulkan();

void PrintExtensions()
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

void PrintLayers()
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

int main(int argc, char** argv)
{
	LoadVulkan();

	PrintExtensions();
	PrintLayers();
	uint32_t version;

	vktCreateWindow(800, 480);
	vktStartMessageLoop();

	vkEnumerateInstanceVersion(&version);

	uint32_t major = VK_VERSION_MAJOR(version);
	uint32_t minor = VK_VERSION_MINOR(version);

	const char* extensions[] =
	{
		"VK_KHR_surface",
		"VK_KHR_win32_surface"
	};

	const char* layers[] = {
		"VK_LAYER_LUNARG_standard_validation"
	};
	
	const char* supportedExtensions[_countof(extensions)] = {0};
	const char* supportedLayers[_countof(layers)] = { 0 };

	const uint32_t enabledExtensions = SelectSupportedExtensions(extensions, supportedExtensions, _countof(extensions));
	const uint32_t enabledLayers = SelectSupportedLayers(layers, supportedLayers, _countof(layers));

	VkApplicationInfo appInfo;
	appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext              = NULL;
	appInfo.pApplicationName   = "vkTest";
	appInfo.applicationVersion = 1;
	appInfo.pEngineName        = "vkTestEngine";
	appInfo.apiVersion         = VK_MAKE_VERSION(1, 0, 0);

	VkInstanceCreateInfo info;
	info.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	info.pNext                   = NULL;
	info.flags                   = 0;
	info.pApplicationInfo        = &appInfo;
	info.enabledLayerCount       = enabledLayers;
	info.ppEnabledLayerNames     = layers;
	info.enabledExtensionCount   = enabledExtensions;
	info.ppEnabledExtensionNames = extensions;

	VkInstance instance;
	vkCreateInstance(&info, NULL, &instance);

	vkDestroyInstance(instance, NULL);

	UnloadVulkan();
	return 0;
}