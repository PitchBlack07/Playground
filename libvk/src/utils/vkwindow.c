
#include <assert.h>
#include <utils/vkwindow.h>
#include <loader/vkloader.h>

static HINSTANCE        hInstance;
static ATOM             WndClass;
static HWND             Window;
static VkInstance       vkInstance;
static VkPhysicalDevice vkPhysicalDevice;
static VkDevice         vkDevice;

static VkPhysicalDevice libvkChooseVkPhysicalDevice(uint32_t count_, const VkPhysicalDevice* devices_)
{
    if (count_) {
        VkPhysicalDeviceProperties p;
        vkGetPhysicalDeviceProperties(devices_[0], &p);

        libvkPrintVkPhysicalDeviceProperties(&p);

        return devices_[0];
    }
    else {
        return NULL;
    }
}

VkResult libvkChooseVkPhysicalDeviceExtensions(const VkPhysicalDevice device_, uint32_t* countOut_, VkExtensionProperties** extensionsOut_)
{
    uint32_t cnt     = 0;
    VkResult success = 0;

    if (!countOut_ || !extensionsOut_) {
        return VK_INCOMPLETE;
    }

    success = vkEnumerateDeviceExtensionProperties(device_, NULL, &cnt, NULL);
    if (LIBVK_VK_FAILED(success)) {
        return success;
    }

    if (cnt == 0) {
        *countOut_      = 0;
        *extensionsOut_ = NULL;

        return VK_SUCCESS;
    }

    *extensionsOut_ = malloc(sizeof(VkExtensionProperties) * cnt);
    if (*extensionsOut_ == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    success = vkEnumerateDeviceExtensionProperties(device_, NULL, &cnt, *extensionsOut_);
    if (LIBVK_VK_FAILED(success)) {
        free(*extensionsOut_);
        *extensionsOut_ = NULL;
        *countOut_      = 0;
        return success;
    }

    *countOut_ = cnt;
    return success;
}

static PFN_libvkChooseVkPhysicalDevice           fnChooseVkPhysicalDevice           = libvkChooseVkPhysicalDevice;
static PFN_libvkChooseVkPhysicalDeviceExtensions fnChooseVkPhysicalDeviceExtensions = libvkChooseVkPhysicalDeviceExtensions;

void libvkSetChooseVkPhysicalDeviceCallback(PFN_libvkChooseVkPhysicalDevice callback_)
{
    if (callback_) {
        fnChooseVkPhysicalDevice = callback_;
    }
    else {
        fnChooseVkPhysicalDevice = libvkChooseVkPhysicalDevice;
    }
}

#define SWITCH_ON_HANDLER(msg__) \
    case msg__:                  \
        return msg__##_HANDLER(hwnd_, msg_, wparam_, lparam_);

#define IMPLEMENT_WINDOW_HANDLER(msg__) LRESULT msg__##_HANDLER(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)

static VkPhysicalDevice libvkLoadPhysicalDevice()
{
    VkPhysicalDevice devbuffer[8] = { 0 };
    uint32_t         devCount     = 0;
    VkResult         success      = VK_SUCCESS;

    success = vkEnumeratePhysicalDevices(vkInstance, &devCount, NULL);
    if (LIBVK_VK_FAILED(success)) {
        return NULL;
    }

    VkPhysicalDevice* const devices = libvkAllocConditional(devCount * sizeof(VkPhysicalDevice), devbuffer, sizeof(devbuffer));
    if (!devices) {
        return NULL;
    }

    success = vkEnumeratePhysicalDevices(vkInstance, &devCount, devices);
    if (LIBVK_VK_FAILED(success)) {
        libvkFreeConditional(devices, devbuffer);
        return NULL;
    }

    if (devCount == 0) {
        libvkFreeConditional(devices, devbuffer);
        return NULL;
    }

    assert(fnChooseVkPhysicalDevice);
    VkPhysicalDevice dev = fnChooseVkPhysicalDevice(devCount, devices);
    libvkFreeConditional(devices, devbuffer);

    return dev;
}

static const char* const* libvkGetDeviceExtensionsFromVkExtensionProperties(uint32_t count_, VkExtensionProperties* properties_)
{
    if (count_ == 0) {
        return NULL;
    }

    const char** extensions = malloc(count_ * sizeof(char*));
    if (extensions == NULL) {
        return NULL;
    }

    for (uint32_t i = 0; i < count_; ++i) {
        extensions[i] = properties_[i].extensionName;
    }

    return extensions;
}

static VkDevice libvkCreateVkDevice(VkPhysicalDevice dev_)
{
    dev_;
    uint32_t               cnt;
    VkExtensionProperties* extensions;
    VkResult               success;

    success = fnChooseVkPhysicalDeviceExtensions(vkPhysicalDevice, &cnt, &extensions);
    if (LIBVK_VK_FAILED(success)) {
        return NULL;
    }

    const char* const* extstr = libvkGetDeviceExtensionsFromVkExtensionProperties(cnt, extensions);
    if (cnt > 0 && extstr == NULL) {
        return NULL;
    }

	//VkDeviceCreateInfo dci;
	//dci.

 //   // doe some other magic
 //   vkCreateDevice(vkPhysicalDevice, NULL, NULL, &device);

    return NULL;
}

IMPLEMENT_WINDOW_HANDLER(WM_CREATE)
{
    vkInstance = libvkGetVkInstance();
    if (!vkInstance) {
        return -1;
    }

    vkPhysicalDevice = libvkLoadPhysicalDevice();
    if (!vkPhysicalDevice) {
        return -1;
    }

    vkDevice = libvkCreateVkDevice(vkPhysicalDevice);
    if (!vkDevice) {
        return -1;
    }

    hwnd_,
        lparam_, msg_, wparam_;
    return 0;
}

IMPLEMENT_WINDOW_HANDLER(WM_PAINT)
{
    PAINTSTRUCT ps;

    hwnd_, msg_, wparam_, lparam_;

    BeginPaint(hwnd_, &ps);
    EndPaint(hwnd_, &ps);

    return 0;
}

IMPLEMENT_WINDOW_HANDLER(WM_CLOSE)
{
    msg_, wparam_, lparam_;
    DestroyWindow(hwnd_);
    return 0;
}

IMPLEMENT_WINDOW_HANDLER(WM_DESTROY)
{
    hwnd_, msg_, wparam_, lparam_;
    PostQuitMessage(0);
    return 0;
}

static LRESULT CALLBACK libvkWindowCallback(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
    switch (msg_) {
        SWITCH_ON_HANDLER(WM_CREATE);
        SWITCH_ON_HANDLER(WM_CLOSE);
        SWITCH_ON_HANDLER(WM_DESTROY);
        SWITCH_ON_HANDLER(WM_PAINT);

    default:
        return DefWindowProc(hwnd_, msg_, wparam_, lparam_);
    }
}

int32_t libvkCreateWindow(const libvkCreateWindowInfo* info_)
{
    WNDCLASS wc;
    RECT     rect;

    if (info_ == NULL) {
        return 0;
    }

    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hInstance     = GetModuleHandle(NULL);
    wc.lpfnWndProc   = libvkWindowCallback;
    wc.lpszClassName = TEXT("libvkWindowClass");
    wc.lpszMenuName  = NULL;
    wc.style         = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;

    WndClass = RegisterClass(&wc);
    if (WndClass == 0) {
        return 0;
    }

    rect.left   = 0;
    rect.right  = info_->width;
    rect.top    = 0;
    rect.bottom = info_->height;

    if (!AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE)) {
        return 0;
    }

    Window = CreateWindow((LPCTSTR)(uintptr_t)WndClass, info_->title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, (rect.right - rect.left), (rect.bottom - rect.top), NULL, NULL, hInstance, NULL);

    if (Window == NULL) {
        return 0;
    }

    return 1;
}

int32_t libvkStartApplication(libvkStartApplicationInfo* info_)
{
    BOOL blocking = info_ ? info_->Unnamed : FALSE;
    MSG  msg      = { 0 };

    if (Window == NULL) {
        return -1;
    }

    ShowWindow(Window, SW_SHOW);

    if (blocking) {
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        return (int32_t)msg.wParam;
    }
    else {
        while (1) {
            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);

                if (msg.message == WM_QUIT) {
                    return (int32_t)msg.wParam;
                }
            }
        }
    }
}
