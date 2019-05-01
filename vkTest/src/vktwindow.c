
#include <Windows.h>
#include <vulkan.h>
#include <vktwindow.h>
#include <vktutils.h>
#include <vktMemory.h>

#define IMAGE_COUNT 3

static ATOM            WndClass;
static HWND            hWnd;
static HINSTANCE       hInstance;
static VkInstance      vkInstance;
static VkDevice        vkDevice;
static VkQueue         vkGraphicsQueue;
static VkSurfaceKHR    vkSurface;
static VkSwapchainKHR  vkSwapchain;
static VkCommandPool   vkCommandPool;
static VkCommandBuffer vkCommandBuffer[IMAGE_COUNT];
static VkImage         vkSwapChainImage[IMAGE_COUNT];
static VkSemaphore     vkSwapChainSemaphore;
static VkSemaphore     vkRenderSemaphore;

static VktVersion          vktVersion;
static VktDeviceExtensions vtkDeviceExtensions;
static VktQueueInfo        vtkQueueInfo;
static VktPresentationInfo vktPresentationInfo;

static const char* vktActiveExtensions[16];
static const char* vktActiveLayers[16];
static uint32_t    vktGraphicsQueueFamilyIndex;
static uint32_t    vktPresentationQueueFamilyIndex;

#define MESSAGE_HANDLER(msg__) static LRESULT msg__##Handler(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
#define INVOKE_MESSAGE_HANDLER(msg__) \
    case msg__:                       \
        return msg__##Handler(hwnd_, msg_, wparam_, lparam_)

extern void LoadVulkan();
extern void UnloadVulkan();

int32_t vktInit(const VktInitilizationInfo* info_)
{
    LoadVulkan();

    vkPrintAvaiableExtensions();
    const uint32_t enabledLayers     = vktSelectSupportedLayers(info_->Layers.Names, vktActiveLayers, info_->Layers.Count);
    const uint32_t enabledExtensions = vktSelectSupportedExtensions(info_->Extensions.Names, vktActiveExtensions, info_->Extensions.Count);

    VkApplicationInfo appInfo;
    appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext              = NULL;
    appInfo.pApplicationName   = "vkTest";
    appInfo.applicationVersion = 1;
    appInfo.pEngineName        = "vkTestEngine";
    appInfo.apiVersion         = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo info;
    info.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.pNext                   = NULL;
    info.flags                   = 0;
    info.pApplicationInfo        = &appInfo;
    info.enabledLayerCount       = enabledLayers;
    info.ppEnabledLayerNames     = vktActiveLayers;
    info.enabledExtensionCount   = enabledExtensions;
    info.ppEnabledExtensionNames = vktActiveExtensions;

    if (VK_SUCCESS != vkCreateInstance(&info, NULL, &vkInstance))
    {
        return -1;
    }

    vktVersion = vktGetVersion();

    return 0;
}

void vktDeinit()
{
    vkDestroyInstance(vkInstance, NULL);
    UnloadVulkan();
}

MESSAGE_HANDLER(WM_CREATE)
{
    VkWin32SurfaceCreateInfoKHR vkCreateInfo;
    vkCreateInfo.sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    vkCreateInfo.pNext     = NULL;
    vkCreateInfo.flags     = 0;
    vkCreateInfo.hinstance = hInstance;
    vkCreateInfo.hwnd      = hwnd_;

    if (vkCreateWin32SurfaceKHR(vkInstance, &vkCreateInfo, NULL, &vkSurface) != VK_SUCCESS)
    {
        return -1;
    }

    VkPhysicalDevice devices[8];
    uint32_t         deviceCount = 8;
    vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices);

    VkPhysicalDeviceProperties devprop;
    vkGetPhysicalDeviceProperties(devices[0], &devprop);

    vtkQueueInfo = vktGetQueueInfos(devices[0]);

    vktGraphicsQueueFamilyIndex     = vktGetGraphicsQueueIndex(&vtkQueueInfo);
    vktPresentationQueueFamilyIndex = vktGetPresentationQueueIndex(&vtkQueueInfo, devices[0], vkSurface);
    vktPresentationInfo             = vktGetPresentationInfo(devices[0], vkSurface);

    if (vktGraphicsQueueFamilyIndex == 0xffffffff)
    {
        return -1;
    }

    VkSurfaceCapabilitiesKHR vkSurfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(devices[0], vkSurface, &vkSurfaceCapabilities);

    VkBool32 win32PresentationSupported = vkGetPhysicalDeviceWin32PresentationSupportKHR(devices[0], vktPresentationQueueFamilyIndex);

    vktPrintDeviceExtensions(devices[0], NULL);

    vtkDeviceExtensions = vktGetDeviceExtensions(devices[0], NULL);

    float gfxQueuePrio = 1.f;

    VkDeviceQueueCreateInfo queueCreateInfo;
    queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.pNext            = NULL;
    queueCreateInfo.queueFamilyIndex = vktGraphicsQueueFamilyIndex;
    queueCreateInfo.queueCount       = 1;
    queueCreateInfo.pQueuePriorities = &gfxQueuePrio;
    queueCreateInfo.flags            = 0;

    const char* deviceExtensions[] = {
        "VK_KHR_swapchain"
    };

    VkDeviceCreateInfo deviceCreationInfo;
    deviceCreationInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreationInfo.pNext                   = NULL;
    deviceCreationInfo.flags                   = 0;
    deviceCreationInfo.queueCreateInfoCount    = 1;
    deviceCreationInfo.pQueueCreateInfos       = &queueCreateInfo;
    deviceCreationInfo.enabledLayerCount       = 0;
    deviceCreationInfo.ppEnabledLayerNames     = NULL;
    deviceCreationInfo.enabledExtensionCount   = _countof(deviceExtensions);
    deviceCreationInfo.ppEnabledExtensionNames = deviceExtensions;
    deviceCreationInfo.pEnabledFeatures        = NULL;

    if (vkCreateDevice(devices[0], &deviceCreationInfo, NULL, &vkDevice) != VK_SUCCESS)
    {
        return -1;
    }

    vkGetDeviceQueue(vkDevice, vktGraphicsQueueFamilyIndex, 0, &vkGraphicsQueue);

    VkSwapchainCreateInfoKHR sci = { 0 };
    sci.sType                    = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    sci.pNext                    = NULL;
    sci.flags                    = 0;
    sci.surface                  = vkSurface;
    sci.minImageCount            = 3;
    sci.imageFormat              = VK_FORMAT_R8G8B8A8_UNORM;
    sci.imageColorSpace          = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    sci.imageExtent              = vkSurfaceCapabilities.currentExtent;
    sci.imageArrayLayers         = 1;
    sci.imageUsage               = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    sci.imageSharingMode         = VK_SHARING_MODE_EXCLUSIVE;
    sci.queueFamilyIndexCount    = 1;
    sci.pQueueFamilyIndices      = &vktGraphicsQueueFamilyIndex;
    sci.preTransform             = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    sci.compositeAlpha           = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    sci.presentMode              = VK_PRESENT_MODE_IMMEDIATE_KHR;
    sci.clipped                  = VK_TRUE;
    sci.oldSwapchain             = VK_NULL_HANDLE;

    VkResult swapchainOk = vkCreateSwapchainKHR(vkDevice, &sci, NULL, &vkSwapchain);

    uint32_t imageCount = IMAGE_COUNT;
    vkGetSwapchainImagesKHR(vkDevice, vkSwapchain, &imageCount, &vkSwapChainImage[0]);

    VkCommandPoolCreateInfo vkCmdPoolCreateInfo;
    vkCmdPoolCreateInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    vkCmdPoolCreateInfo.pNext            = NULL;
    vkCmdPoolCreateInfo.queueFamilyIndex = vktGraphicsQueueFamilyIndex;
    vkCmdPoolCreateInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    vkCreateCommandPool(vkDevice, &vkCmdPoolCreateInfo, NULL, &vkCommandPool);

    VkCommandBufferAllocateInfo cmdBufferAllocInfo;
    cmdBufferAllocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBufferAllocInfo.pNext              = NULL;
    cmdBufferAllocInfo.commandPool        = vkCommandPool;
    cmdBufferAllocInfo.commandBufferCount = IMAGE_COUNT;
    cmdBufferAllocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    vkAllocateCommandBuffers(vkDevice, &cmdBufferAllocInfo, vkCommandBuffer);

    VkSemaphoreCreateInfo semaphoreCreateInfo;
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCreateInfo.pNext = NULL;
    semaphoreCreateInfo.flags = 0;

    vkCreateSemaphore(vkDevice, &semaphoreCreateInfo, NULL, &vkSwapChainSemaphore);
    vkCreateSemaphore(vkDevice, &semaphoreCreateInfo, NULL, &vkRenderSemaphore);

    return 0;
}

MESSAGE_HANDLER(WM_CLOSE)
{
    vkDestroySemaphore(vkDevice, vkRenderSemaphore, NULL);
    vkDestroySemaphore(vkDevice, vkSwapChainSemaphore, NULL);

    vkDestroyCommandPool(vkDevice, vkCommandPool, NULL);
    vktFreeQueueInfo(&vtkQueueInfo);
    vktFreeDeviceExtensions(&vtkDeviceExtensions);
    vktFreePresentationInfo(&vktPresentationInfo);

    if (vkSwapchain)
    {
        vkDestroySwapchainKHR(vkDevice, vkSwapchain, NULL);
    }

    if (vkSurface)
    {
        vkDestroySurfaceKHR(vkInstance, vkSurface, NULL);
    }

    if (vkDevice)
    {
        vkDeviceWaitIdle(vkDevice);
        vkDestroyDevice(vkDevice, NULL);
    }

    DestroyWindow(hwnd_);
    return 0;
}

MESSAGE_HANDLER(WM_DESTROY)
{
    PostQuitMessage(0);
    return 0;
}

MESSAGE_HANDLER(WM_PAINT)
{
    PAINTSTRUCT ps;
    BeginPaint(hwnd_, &ps);
    EndPaint(hwnd_, &ps);

    if (vkDevice == VK_NULL_HANDLE)
    {
        return 0;
    }

    uint32_t vkImageIndex = 0;
    vkAcquireNextImageKHR(vkDevice, vkSwapchain, UINT64_MAX, vkSwapChainSemaphore, VK_NULL_HANDLE, &vkImageIndex);

    VkImage         img           = vkSwapChainImage[vkImageIndex];
    VkCommandBuffer commandBuffer = vkCommandBuffer[vkImageIndex];

    VkImageSubresourceRange range;
    range.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    range.baseArrayLayer = 0;
    range.baseMipLevel   = 0;
    range.layerCount     = 1;
    range.levelCount     = 1;

    VkImageMemoryBarrier imb;
    imb.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imb.pNext               = NULL;
    imb.dstAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT;
    imb.srcAccessMask       = VK_ACCESS_MEMORY_READ_BIT;
    imb.oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
    imb.newLayout           = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imb.srcQueueFamilyIndex = vktGraphicsQueueFamilyIndex;
    imb.dstQueueFamilyIndex = vktGraphicsQueueFamilyIndex;
    imb.image               = img;
    imb.subresourceRange    = range;

    VkCommandBufferBeginInfo beginInfo;
    beginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext            = NULL;
    beginInfo.pInheritanceInfo = NULL;
    beginInfo.flags            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &imb);
    VkClearColorValue value;
    value.float32[0] = 1.f;
    value.float32[1] = 0;
    value.float32[2] = 0;
    value.float32[3] = 1.f;

    // Transition to VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL

    vkCmdClearColorImage(commandBuffer, img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &value, 1, &range);

    // Transition to  VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    imb.dstAccessMask       = VK_ACCESS_MEMORY_READ_BIT;
    imb.srcAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT;
    imb.oldLayout           = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imb.newLayout           = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    imb.srcQueueFamilyIndex = vktGraphicsQueueFamilyIndex;
    imb.dstQueueFamilyIndex = vktGraphicsQueueFamilyIndex;
    imb.image               = img;
    imb.subresourceRange    = range;
    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 0, NULL, 1, &imb);
    vkEndCommandBuffer(commandBuffer);

    //
    // Warning: SubmitInfo should specify a signal semaphore to signal which vkQueuePresentKHR should wait on
    //

    VkPipelineStageFlags waitflags[] = { VK_PIPELINE_STAGE_TRANSFER_BIT };
    VkSubmitInfo         submitInfo;
    submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext                = NULL;
    submitInfo.commandBufferCount   = 1;
    submitInfo.pCommandBuffers      = &commandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = &vkRenderSemaphore;
    submitInfo.waitSemaphoreCount   = 1;
    submitInfo.pWaitSemaphores      = &vkSwapChainSemaphore;
    submitInfo.pWaitDstStageMask    = waitflags;

    vkQueueSubmit(vkGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);

    VkResult         results;
    VkPresentInfoKHR presentInfo;
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext              = NULL;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = &vkRenderSemaphore;
    presentInfo.swapchainCount     = 1;
    presentInfo.pSwapchains        = &vkSwapchain;
    presentInfo.pImageIndices      = &vkImageIndex;
    presentInfo.pResults           = &results;

    vkQueuePresentKHR(vkGraphicsQueue, &presentInfo);

    return 0;
}

static LRESULT CALLBACK vktWndProc(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
    switch (msg_)
    {
        INVOKE_MESSAGE_HANDLER(WM_CREATE);
        INVOKE_MESSAGE_HANDLER(WM_CLOSE);
        INVOKE_MESSAGE_HANDLER(WM_DESTROY);
        INVOKE_MESSAGE_HANDLER(WM_PAINT);

    default:
        return DefWindowProc(hwnd_, msg_, wparam_, lparam_);
    }
}

int32_t vktCreateWindow(uint32_t width_, uint32_t height_)
{
    hInstance = GetModuleHandle(NULL);
    WNDCLASS wc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hInstance     = hInstance;
    wc.lpfnWndProc   = vktWndProc;
    wc.lpszClassName = TEXT("vktApplicationWindow");
    wc.lpszMenuName  = NULL;
    wc.style         = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;

    WndClass = RegisterClass(&wc);
    if (WndClass == 0)
    {
        return 0;
    }

    hWnd = CreateWindow((LPCTSTR)WndClass, TEXT("vtkWindow"), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, width_, height_, NULL, NULL, hInstance, NULL);

    if (hWnd == NULL)
    {
        return 0;
    }

    return 1;
}

int32_t vktStartMessageLoop()
{
    ShowWindow(hWnd, SW_SHOW);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int32_t)msg.lParam;
}
