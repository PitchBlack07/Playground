
#include <loader/vkloader.h>

#include <assert.h>
#include <Windows.h>

static HMODULE    vkLibrary  = NULL;
static VkInstance vkInstance = NULL;

extern void libvk_load_default_functions(PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr_);
extern void libvk_load_instance_functions(VkInstance instance_);

VkResult libvk_load_vulkan(const VkInstanceCreateInfo* vkCreateInfo_, VkAllocationCallbacks* vkAllocator_, uint32_t* vkVersionOut_)
{
    VkResult success = VK_SUCCESS;

    if (!vkLibrary) {
        vkLibrary = LoadLibrary(TEXT("vulkan-1.dll"));
        if (vkLibrary == NULL) {
            return VK_ERROR_INITIALIZATION_FAILED;
        }

        libvk_load_default_functions((PFN_vkGetInstanceProcAddr )GetProcAddress(vkLibrary, TEXT("vkGetInstanceProcAddr")));
    }

    if (vkVersionOut_) {
        success = vkEnumerateInstanceVersion(vkVersionOut_);
    }

    if (success != VK_SUCCESS || vkCreateInfo_ == NULL) {
        return success;
    }

    success = vkCreateInstance(vkCreateInfo_, vkAllocator_, &vkInstance);
    if (success != VK_SUCCESS) {
        return success;
    }

	libvk_load_instance_functions(vkInstance);

    return success;
}

void libvk_unload_vulkan(VkAllocationCallbacks* vkAllocator_)
{
    if (vkInstance) {
        vkDestroyInstance(vkInstance, vkAllocator_);
        vkInstance = NULL;
    }

    FreeLibrary(vkLibrary);
}
