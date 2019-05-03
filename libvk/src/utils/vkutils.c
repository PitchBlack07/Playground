
#include <utils/vkutils.h>
#include <loader/vk.h>
#include <stdio.h>

void* libvk_alloc_conditional(size_t size_, void* buffer_, size_t bufferSize_)
{
    if (size_ <= bufferSize_)
        return buffer_;

    return malloc(size_);
}

void libvk_free_conditional(void* ptr_, void* buffer_)
{
    if (ptr_ != buffer_)
        free(ptr_);
}

VkResult libvk_print_layers_and_extensions_to_console()
{
    uint32_t          lcount = 0, i = 0;
    VkLayerProperties lbuffer[8] = { 0 };

    VkResult success = vkEnumerateInstanceLayerProperties(&lcount, NULL);
    if (LIBVK_VK_FAILED(success)) {
        return success;
    }

    VkLayerProperties* const vkLayers = libvk_alloc_conditional(sizeof(VkLayerProperties) * lcount, lbuffer, sizeof(lbuffer));
    if (!vkLayers) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    success = vkEnumerateInstanceLayerProperties(&lcount, vkLayers);
    if (LIBVK_VK_FAILED(success)) {
        libvk_free_conditional(vkLayers, lbuffer);
        return success;
    }

    uint32_t vkVersion;
    vkEnumerateInstanceVersion(&vkVersion);

    printf("Name:        Vulkan Runtime\n");
    printf("Version:     %u.%u.%u\n", VK_VERSION_MAJOR(vkVersion), VK_VERSION_MINOR(vkVersion), VK_VERSION_PATCH(vkVersion));
    printf("Extensions:  ");
    libvk_print_extensions_to_console(NULL);
    printf("\n---------------------------------------------------------------\n");

    for (i = 0; i < lcount; ++i) {
        printf("Name:        %s\n", vkLayers[i].layerName);
        printf("Description: %s\n", vkLayers[i].description);
        printf("Version:     %u\n", vkLayers[i].implementationVersion);
        printf("Extensions:  ");
        libvk_print_extensions_to_console(vkLayers[i].layerName);
        printf("\n---------------------------------------------------------------\n");
    }

    libvk_free_conditional(vkLayers, lbuffer);
    return success;
}

VkResult libvk_print_extensions_to_console(const char* layer_)
{
    uint32_t              ecount = 0, i = 0;
    VkExtensionProperties ebuffer[32] = { 0 };

    VkResult success = vkEnumerateInstanceExtensionProperties(layer_, &ecount, NULL);
    if (LIBVK_VK_FAILED(success)) {
        return success;
    }

    VkExtensionProperties* const vkExtensions = libvk_alloc_conditional(sizeof(VkExtensionProperties) * ecount, ebuffer, sizeof(ebuffer));
    if (!vkExtensions) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    success = vkEnumerateInstanceExtensionProperties(layer_, &ecount, vkExtensions);
    if (LIBVK_VK_FAILED(success)) {
        libvk_free_conditional(vkExtensions, ebuffer);
        return success;
    }

    if (ecount == 0) {
        libvk_free_conditional(vkExtensions, ebuffer);
        return success;
    }

    for (i = 0; i < ecount - 1; ++i) {
        printf("%s, ", vkExtensions[i].extensionName);
    }

    printf("%s", vkExtensions[ecount - 1].extensionName);

	libvk_free_conditional(vkExtensions, ebuffer);
    return success;
}