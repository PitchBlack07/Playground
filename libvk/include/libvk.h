#ifndef LIBVK_H_
#define LIBVK_H_

#include <loader/vkloader.h>
#include <utils/vkutils.h>
#include <utils/vkwindow.h>

VkResult libvk_get_supported_extensions(const char* layer_, VkExtensionProperties** propertiesOut_, uint32_t* countOut_);
VkBool32 libvk_is_extension_supported(const char* ext_, const VkExtensionProperties* extensions_, uint32_t count_);
VkResult libvk_choose_supported_extensions(const char* layer_, const char** extensionsInOut_, uint32_t* countIntOut_);

VkResult libvk_choose_supported_extensions2(const char* layer_, uint32_t extensionCount_, const char* const* extensions_, uint32_t* bufferCountInOut_, const char** extensionsOut_);

VkResult libvk_get_supported_layers(VkLayerProperties** propertiesOut_, uint32_t* countOut_);
VkBool32 libvk_is_layer_supported(const char* layer_, const VkLayerProperties* layers_, uint32_t count_);
VkResult libvk_choose_supported_layers(const char** layersInOut_, uint32_t* countInOut_);

typedef struct libvk_application_t_
{
    VkInstance vkInstance;
    HWND       hwnd;
    HDC        hdc;
} libvk_application_t;

libvk_application_t* libvk_create_application(const char* name_, uint32_t layerCount_, const char* const* layers_, uint32_t extensionCount_, const char* const* extensions_);
void                 livk_destroy_application(libvk_application_t* app_);
void*                libvk_alloc_conditional(size_t size_, void* buffer_, size_t bufferSize_);
void                 libvk_free_conditional(void* ptr_, void* buffer_);

#endif