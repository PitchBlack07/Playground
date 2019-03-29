
#include <Windows.h>
#include <vulkan.h>
#include <vktwindow.h>
#include <vktutils.h>

static ATOM         WndClass;
static HWND         hWnd;
static HINSTANCE    hInstance;
static VkInstance   vkInstance;
static VkSurfaceKHR vkSurface;

static VktVersion  vktVersion;
static const char* vktActiveExtensions[16];
static const char* vktActiveLayers[16];

#define MESSAGE_HANDLER(msg__) static LRESULT msg__##Handler(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
#define INVOKE_MESSAGE_HANDLER(msg__) case msg__: return msg__##Handler(hwnd_, msg_, wparam_, lparam_)

extern void LoadVulkan();
extern void UnloadVulkan();

int32_t vktInit(const VktInitilizationInfo* info_) {
	
	LoadVulkan();

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

	if (vkCreateWin32SurfaceKHR(vkInstance, &vkCreateInfo, NULL, &vkSurface) == VK_SUCCESS)
	{
		return 0;
	}

	return -1;
}

MESSAGE_HANDLER(WM_CLOSE)
{
	if (vkSurface)
	{
		vkDestroySurfaceKHR(vkInstance, vkSurface, NULL);
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