
#pragma once

#ifndef _LIBVK_VKLOADER_H_
#define _LIBVK_VKLOADER_H_

#include <vulkan.h>
#include "vk.h"

VkResult libvk_load_vulkan(const VkInstanceCreateInfo* vkCreateInfo_, VkAllocationCallbacks* vkAllocator_, uint32_t* vkVersionOut_);
void     libvk_unload_vulkan(VkAllocationCallbacks* vkAllocator_);

#endif
