#include <stdint.h>
#include <vulkan.h>
#include <Windows.h>
#include <stdio.h>
#include <vktMemory.h>
#include <vktutils.h>
#include <vktwindow.h>

extern void LoadVulkan();
extern void UnloadVulkan();

static const char* extensions[] =
{
	"VK_KHR_surface",
	"VK_KHR_win32_surface"
};

static const char* layers[] = {
	"VK_LAYER_LUNARG_standard_validation"
};

int main(int argc, char** argv)
{
	VktInitilizationInfo info;
	info.Extensions.Names = extensions;
	info.Extensions.Count = _countof(extensions);
	info.Layers.Names     = layers;
	info.Layers.Count     = _countof(layers);
	
	vktInit(&info);
	vktCreateWindow(800, 480);
	vktStartMessageLoop();

	//PrintExtensions();
	//PrintLayers();
	//uint32_t version;

	//vkEnumerateInstanceVersion(&version);

	//uint32_t major = VK_VERSION_MAJOR(version);
	//uint32_t minor = VK_VERSION_MINOR(version);


	//
	//const char* supportedExtensions[_countof(extensions)] = {0};
	//const char* supportedLayers[_countof(layers)] = { 0 };

	//const uint32_t enabledExtensions = SelectSupportedExtensions(extensions, supportedExtensions, _countof(extensions));
	//const uint32_t enabledLayers = SelectSupportedLayers(layers, supportedLayers, _countof(layers));

	return 0;
}