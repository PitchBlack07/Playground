
#pragma once
#ifndef _LIBVK_VK_WINDOW_H_
#define _LIBVK_VK_WINDOW_H_

#include <utils/vkutils.h>

#include <Windows.h>

typedef struct libvkCreateWindowInfo_
{
    LPCTSTR  title;
    uint32_t width;
    uint32_t height;
} libvkCreateWindowInfo;

int32_t libvkCreateWindow(const libvkCreateWindowInfo* info_);

typedef VkPhysicalDevice (*PFN_libvkChooseVkPhysicalDevice)(uint32_t count_, const VkPhysicalDevice* devices_);

typedef VkResult (*PFN_libvkChooseVkPhysicalDeviceExtensions)(const VkPhysicalDevice device_, uint32_t* countOut_, VkExtensionProperties** extensionsOut_);

void libvkSetChooseVkPhysicalDeviceCallback(PFN_libvkChooseVkPhysicalDevice callback_);

typedef struct libvkStartApplicationInfo_
{
    BOOL Unnamed;
} libvkStartApplicationInfo;

int32_t libvkStartApplication(libvkStartApplicationInfo* info_);

#endif