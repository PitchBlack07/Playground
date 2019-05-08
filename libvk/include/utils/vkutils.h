
#pragma once
#ifndef _LIBVK_VK_UTILS_H_
#define _LIBVK_VK_UTILS_H_

#include <vulkan.h>

#ifndef LIBVK_VK_FAILED
#define LIBVK_VK_FAILED(v__) (v__ != VK_SUCCESS)
#endif

#ifndef LIBVK_DECLARE_HANDLE
#define LIBVK_DECLARE_HANDLE(name__) typedef struct name__##__* name__
#endif

void* libvk_alloc_conditional(size_t size_, void* buffer_, size_t bufferSize_);
void  libvk_free_conditional(void* ptr_, void* buffer_);

inline void* libvkAllocConditional(size_t size_, void* buffer_, size_t bufferSize_)
{
    return libvk_alloc_conditional(size_, buffer_, bufferSize_);
}

inline void libvkFreeConditional(void* ptr_, void* buffer_)
{
    libvk_free_conditional(ptr_, buffer_);
}

VkResult libvkCreateDefaultDeviceAndQueues(VkDevice* devOut_, )

VkResult libvk_print_layers_and_extensions_to_console();
VkResult libvk_print_extensions_to_console(const char* layer_);
VkResult libvkPrintVkPhysicalDeviceProperties(const VkPhysicalDeviceProperties* properties_);

#endif