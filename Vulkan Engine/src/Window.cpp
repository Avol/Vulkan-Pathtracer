#include "Window.h"
#include "Renderer.h"
#include "Shared.h"

#include <assert.h>

Window::Window( Renderer * renderer, uint32_t size_x, uint32_t size_y, std::string name )
{
	std::cout << "-------------------------------------- Creating Window Surface -----------------------------------" << std::endl;

	_renderer = renderer;
	_surface_size_x = size_x;
	_surface_size_y = size_y;
	_window_name = name;

	_InitOSWindow();
	_InitSurface();

	_presentation = new Presentation(renderer, _surface, _surface_capabilities, _surface_format, _surface_size_x, _surface_size_y);

	/*
	_InitSwapChain();
	_InitSwapChainImages();

	_CreateCommandPoolAndBuffers();
	_CreateSemaphores();

	_CreatePresentationSampler( renderer );*/
}

Window::~Window()
{
	_DeInitOSWindow();
	_DeInitSurface();
}



void Window::SetTitle(const char* title)
{

}

void Window::Close()
{
	_window_should_run = false;
}

bool Window::Update()
{
	_UpdateOSWindow();
	return _window_should_run;
}


HWND Window::GetHandle()
{
	return _win32_window;
}

Presentation * Window::GetPresentation()
{
	return _presentation;
}


void Window::_InitSurface()
{
	_InitOSSurface();

	auto gpu		= _renderer->GetGPU();

	// check if WSI extension supported.
	VkBool32 WSI_EXTENSION_SUPPORTED;
	vkGetPhysicalDeviceSurfaceSupportKHR( gpu, _renderer->GetGraphicsFamilyIndex(), _surface, &WSI_EXTENSION_SUPPORTED );
	if ( !WSI_EXTENSION_SUPPORTED ) {
		assert(0 && "WSI Extension not supported");
		std::exit( -1 );
	}

	// get device surface capabilities & formats
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR( gpu, _surface, &_surface_capabilities );
	if ( _surface_capabilities.currentExtent.width < UINT32_MAX ) {
		_surface_size_x		= _surface_capabilities.currentExtent.width;
		_surface_size_y		= _surface_capabilities.currentExtent.height;
	}

	{
		uint32_t format_count = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR( gpu, _surface, &format_count, nullptr );

		// check if surface has formats.
		if (format_count == 0)
		{
			assert(0 && "Surface Formats are missing.");
			std::exit(-1);
		}

		// Get GPU surface formats
		std::vector<VkSurfaceFormatKHR> formats ( format_count );
		vkGetPhysicalDeviceSurfaceFormatsKHR( gpu, _surface, &format_count, formats.data() );

		// if surface doesn't care about the format.
		// ussualy its not the case.
		if (formats[ 0 ].format == VK_FORMAT_UNDEFINED)
		{
			_surface_format.format			= VK_FORMAT_B8G8R8A8_UNORM;
			_surface_format.colorSpace		= VK_COLORSPACE_SRGB_NONLINEAR_KHR;
		}
		// otherwise just set it to the first format on the list.
		// since it's the recommended option
		else
		{
			_surface_format					= formats[0];
		}
	}
}

void Window::_DeInitSurface()
{
	vkDestroySurfaceKHR(_renderer->GetInstance(), _surface, nullptr);
}


#if VK_USE_PLATFORM_WIN32_KHR

// Microsoft Windows specific versions of window functions
LRESULT CALLBACK WindowsEventHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Window * window = reinterpret_cast<Window*>(
		GetWindowLongPtrW(hWnd, GWLP_USERDATA));

	switch (uMsg) {
	case WM_CLOSE:
		window->Close();
		return 0;
	case WM_SIZE:
		// we get here if the window has changed size, we should rebuild most
		// of our window resources before rendering to this window again.
		// ( no need for this because our window sizing by hand is disabled )
		break;
	default:
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

uint64_t	Window::_win32_class_id_counter = 0;

void Window::_InitOSWindow()
{
	WNDCLASSEX win_class{};
	assert(_surface_size_x > 0);
	assert(_surface_size_y > 0);

	_win32_instance = GetModuleHandle(nullptr);
	_win32_class_name = _window_name + "_" + std::to_string(_win32_class_id_counter);
	_win32_class_id_counter++;

	// Initialize the window class structure:
	win_class.cbSize = sizeof(WNDCLASSEX);
	win_class.style = CS_HREDRAW | CS_VREDRAW;
	win_class.lpfnWndProc = WindowsEventHandler;
	win_class.cbClsExtra = 0;
	win_class.cbWndExtra = 0;
	win_class.hInstance = _win32_instance; // hInstance
	win_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	win_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	win_class.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	win_class.lpszMenuName = NULL;
	win_class.lpszClassName = _win32_class_name.c_str();
	win_class.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
	// Register window class:
	if (!RegisterClassEx(&win_class)) {
		// It didn't work, so try to give a useful error:
		assert(0 && "Cannot create a window in which to draw!\n");
		fflush(stdout);
		std::exit(-1);
	}

	DWORD ex_style = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

	// Create window with the registered class:
	RECT wr = { 0, 0, LONG(_surface_size_x), LONG(_surface_size_y) };
	AdjustWindowRectEx(&wr, style, FALSE, ex_style);
	_win32_window = CreateWindowEx(0,
		_win32_class_name.c_str(),		// class name
		_window_name.c_str(),			// app name
		style,							// window style
		CW_USEDEFAULT, CW_USEDEFAULT,	// x/y coords
		wr.right - wr.left,				// width
		wr.bottom - wr.top,				// height
		NULL,							// handle to parent
		NULL,							// handle to menu
		_win32_instance,				// hInstance
		NULL);							// no extra parameters
	if (!_win32_window) {
		// It didn't work, so try to give a useful error:
		assert(1 && "Cannot create a window in which to draw!\n");
		fflush(stdout);
		std::exit(-1);
	}
	SetWindowLongPtr(_win32_window, GWLP_USERDATA, (LONG_PTR)this);

	ShowWindow(_win32_window, SW_SHOW);
	SetForegroundWindow(_win32_window);
	SetFocus(_win32_window);
}

void Window::_DeInitOSWindow()
{
	DestroyWindow(_win32_window);
	UnregisterClass(_win32_class_name.c_str(), _win32_instance);
}

void Window::_UpdateOSWindow()
{
	MSG msg;
	if (PeekMessage(&msg, _win32_window, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void Window::_InitOSSurface()
{
	VkWin32SurfaceCreateInfoKHR create_info {};
	create_info.sType			= VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	create_info.hwnd			= _win32_window;
	create_info.hinstance		= _win32_instance;
	vkCreateWin32SurfaceKHR( _renderer->GetInstance(), &create_info, nullptr, &_surface);
}

#endif
