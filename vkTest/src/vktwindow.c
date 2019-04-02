
#include <Windows.h>
#include <vulkan.h>
#include <vktwindow.h>
#include <vktutils.h>
#include <vktMemory.h>

static ATOM         WndClass;
static HWND         hWnd;
static HINSTANCE    hInstance;
static VkInstance   vkInstance;
static VkDevice     vkDevice;
static VkQueue      vkGraphicsQueue;
static VkSurfaceKHR vkSurface;

static VktVersion          vktVersion;
static VktDeviceExtensions vtkDeviceExtensions;
static VktQueueInfo        vtkQueueInfo;
static VktPresentationInfo vktPresentationInfo;

static const char*         vktActiveExtensions[16];
static const char*         vktActiveLayers[16];
static uint32_t            vktGraphicsQueueFamilyIndex;
static uint32_t            vktPresentationQueueFamilyIndex;

#define MESSAGE_HANDLER(msg__) static LRESULT msg__##Handler(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
#define INVOKE_MESSAGE_HANDLER(msg__) case msg__: return msg__##Handler(hwnd_, msg_, wparam_, lparam_)

extern void LoadVulkan();
extern void UnloadVulkan();

int32_t vktInit(const VktInitilizationInfo* info_) {
	
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
	uint32_t deviceCount = 8;
	vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices);

	VkPhysicalDeviceProperties devprop;
	vkGetPhysicalDeviceProperties(devices[0], &devprop);

	vtkQueueInfo = vktGetQueueInfos(devices[0]);

	vktGraphicsQueueFamilyIndex     = vktGetGraphicsQueueIndex(&vtkQueueInfo);
	vktPresentationQueueFamilyIndex = vktGetPresentationQueueIndex(&vtkQueueInfo, devices[0], vkSurface);
	vktPresentationInfo             = vktGetPresentationInfo(devices[0], vkSurface);

	VkSurfaceCapabilitiesKHR vkSurfaceCapabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(devices[0], vkSurface, &vkSurfaceCapabilities);

	VkBool32 win32PresentationSupported = vkGetPhysicalDeviceWin32PresentationSupportKHR(devices[0], vktPresentationQueueFamilyIndex);
	
	if (vktGraphicsQueueFamilyIndex == 0xffffffff) {
		return -1;
	}

	vktPrintDeviceExtensions(devices[0], vktActiveLayers[0]);

	vtkDeviceExtensions = vktGetDeviceExtensions(devices[0], vktActiveLayers[0]);
	
	float gfxQueuePrio = 1.f;

	VkDeviceQueueCreateInfo queueCreateInfo;
	queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.pNext            = NULL;
	queueCreateInfo.queueFamilyIndex = vktGraphicsQueueFamilyIndex;
	queueCreateInfo.queueCount       = 1;
	queueCreateInfo.pQueuePriorities = &gfxQueuePrio;
	

	VkDeviceCreateInfo deviceCreationInfo;
	deviceCreationInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreationInfo.pNext                   = NULL;
	deviceCreationInfo.flags                   = 0;
	deviceCreationInfo.queueCreateInfoCount    = 1;
	deviceCreationInfo.pQueueCreateInfos       = &queueCreateInfo;
	deviceCreationInfo.enabledLayerCount       = 0;
	deviceCreationInfo.ppEnabledLayerNames     = NULL;
	deviceCreationInfo.enabledExtensionCount   = 0;
	deviceCreationInfo.ppEnabledExtensionNames = NULL;
	deviceCreationInfo.pEnabledFeatures        = NULL;

	if (vkCreateDevice(devices[0], &deviceCreationInfo, NULL, &vkDevice) != VK_SUCCESS) {
		return -1;
	}

	vkGetDeviceQueue(vkDevice, vktGraphicsQueueFamilyIndex, 0, &vkGraphicsQueue);

	return 0;
}

MESSAGE_HANDLER(WM_CLOSE)
{
	vktFreeQueueInfo(&vtkQueueInfo);
	vktFreeDeviceExtensions(&vtkDeviceExtensions);
	vktFreePresentationInfo(&vktPresentationInfo);

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

static LRESULT CALLBACK vktWndProc(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
	switch (msg_)
	{
	INVOKE_MESSAGE_HANDLER(WM_CREATE);
	INVOKE_MESSAGE_HANDLER(WM_CLOSE);
	INVOKE_MESSAGE_HANDLER(WM_DESTROY);

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
	if (WndClass == 0) {
		return 0;
	}

	hWnd = CreateWindow((LPCTSTR)WndClass, TEXT("vtkWindow"), WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, CW_USEDEFAULT, width_, height_, NULL, NULL, hInstance, NULL);

	if (hWnd == NULL) {
		return 0;
	}

	return 1;
}

int32_t vktStartMessageLoop() {

	ShowWindow(hWnd, SW_SHOW);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int32_t) msg.lParam;
}

