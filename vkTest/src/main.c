#include <stdint.h>
#include <Windows.h>
#include <stdio.h>
#include <vktMemory.h>
#include <vktutils.h>
#include <vktwindow.h>
#include <libvk.h>

static const char* extensions[] = {
    "VK_KHR_surface",
    "VK_KHR_win32_surface"
};

static const char* layers[] = {
    "VK_LAYER_LUNARG_standard_validation"
};

int main(int argc, char** argv)
{
    uint32_t ecount = _countof(extensions);
    uint32_t lcount = _countof(layers);
    uint32_t vkVersion;

    const VkResult success = libvk_load_vulkan(NULL, NULL, &vkVersion);
    if (success != VK_SUCCESS) {
        return -1;
	}

	libvk_print_layers_and_extensions_to_console();

	//libvk_create_application("myapp", lcount, layers, ecount, extensions);

    //int32_t retVal = 0;

    //VktInitilizationInfo info;
    //info.Extensions.Names = extensions;
    //info.Extensions.Count = _countof(extensions);
    //info.Layers.Names =  layers;
    //info.Layers.Count = _countof(layers);
    //
    //vktInit(&info);
    //vktCreateWindow(800, 480);
    //retVal = vktStartMessageLoop();
    //vktDeinit();

	libvk_unload_vulkan(NULL);
    return 0;
    //return retVal;
}