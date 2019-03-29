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
	int32_t retVal;

	VktInitilizationInfo info;
	info.Extensions.Names = extensions;
	info.Extensions.Count = _countof(extensions);
	info.Layers.Names     = layers;
	info.Layers.Count     = _countof(layers);
	
	vktInit(&info);
	vktCreateWindow(800, 480);
	retVal = vktStartMessageLoop();
	vktDeinit();

	return retVal;
}