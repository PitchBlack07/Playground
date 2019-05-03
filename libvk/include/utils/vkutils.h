
#pragma once
#ifndef _LIBVK_VK_UTILS_H_
#define _LIBVK_VK_UTILS_H_

#include <vulkan.h>

#ifndef LIBVK_VK_FAILED
#define LIBVK_VK_FAILED(v__)(v__ != VK_SUCCESS)
#endif

void* libvk_alloc_conditional(size_t size_, void* buffer_, size_t bufferSize_);
void  libvk_free_conditional(void* ptr_, void* buffer_);

VkResult libvk_print_layers_and_extensions_to_console();
VkResult libvk_print_extensions_to_console(const char* layer_);

#endif