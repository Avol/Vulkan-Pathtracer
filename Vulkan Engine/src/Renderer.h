#pragma once

#include <vector>
#include "Platform.h"
#include "Window.h"

class Window;
class Renderer
{
private:

	VkInstance							_instance						= VK_NULL_HANDLE;
	VkPhysicalDevice					_gpu							= VK_NULL_HANDLE;
	VkDevice							_device							= VK_NULL_HANDLE;
	VkQueue								_queue							= VK_NULL_HANDLE;
	VkQueue								_compute_queue					= VK_NULL_HANDLE;
	VkPhysicalDeviceProperties			_gpu_properties					= {};
	VkPhysicalDeviceMemoryProperties	_memory_properties				= {};

	uint32_t							_graphics_family_index			= 0;
	uint32_t							_compute_family_index			= 0;

	std::vector<const char*>			_instance_layers;
	std::vector<const char*>			_instance_extensions;

	std::vector<const char*>			_device_layers;
	std::vector<const char*>			_device_extensions;

	VkDebugReportCallbackEXT			_debug_report					= VK_NULL_HANDLE;

	Window					*			_window;
public:

	Renderer();
	~Renderer();

	VkDevice							GetDevice();
	VkPhysicalDevice					GetGPU();
	VkInstance							GetInstance();
	uint32_t							GetGraphicsFamilyIndex();
	uint32_t							GetComputeFamilyIndex();
	VkQueue								GetQueue();
	VkQueue								GetComputeQueue();
	VkPhysicalDeviceProperties			GetGPUProperties();
	uint32_t							GetGPUMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32 *memTypeFound = nullptr);
	

	Window					*		OpenWindow(uint32_t size_x, uint32_t size_y, std::string name);
	Window					*		Renderer::GetWindow();

	bool							Run();
private:

	void _InitInstance();
	void _DeInitInstance();

	void _InitDevice();
	void _DeInitDevice();

	void _SetupDebug();
	void _InitDebug();
	void _DeInitDebug();

	void _SetupLayersAndExtensions();
};

