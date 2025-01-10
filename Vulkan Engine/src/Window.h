#pragma once

#include <string>
#include <vector>

#include "Platform.h"
#include "BUILD_OPTIONS.h"
#include "Renderer.h"
#include "../Presentation.h"

class Renderer;
class Presentation;

class Window
{
private:
	uint32_t							_surface_size_x = 512;
	uint32_t							_surface_size_y = 512;
	std::string							_window_name;

	bool								_window_should_run = true;

	Renderer				*			_renderer;
	Presentation			*			_presentation;

	VkSurfaceKHR						_surface;

	VkSurfaceFormatKHR					_surface_format = {};
	VkSurfaceCapabilitiesKHR			_surface_capabilities = {};

public:
	Window( Renderer * renderer, uint32_t size_x, uint32_t size_y, std::string name);
	~Window();

	void								SetTitle(const char* title);
	void								Close();
	bool								Update();

	HWND								GetHandle();
	Presentation		*				GetPresentation();

private:
	void								_InitOSWindow();
	void								_DeInitOSWindow();
	void								_UpdateOSWindow();

	void								_InitOSSurface();
	void								_InitSurface();
	void								_DeInitSurface();

#if VK_USE_PLATFORM_WIN32_KHR
	HINSTANCE							_win32_instance = NULL;
	HWND								_win32_window = NULL;
	std::string							_win32_class_name;
	static uint64_t						_win32_class_id_counter;
#elif VK_USE_PLATFORM_XCB_KHR
	xcb_connection_t				*	_xcb_connection = nullptr;
	xcb_screen_t					*	_xcb_screen = nullptr;
	xcb_window_t						_xcb_window = 0;
	xcb_intern_atom_reply_t			*	_xcb_atom_window_reply = nullptr;
#endif
};