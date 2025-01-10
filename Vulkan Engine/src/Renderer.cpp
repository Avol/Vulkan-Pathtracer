#include "Renderer.h"
#include "Shared.h"

#include <iostream>
#include <cstdlib>
#include <assert.h>
#include <vector>
#include <sstream>



Renderer::Renderer()
{
	std::cout << "-------------------------------------- Creating Renderer -----------------------------------" << std::endl;

	_SetupLayersAndExtensions();
	_SetupDebug();
	_InitInstance();
	_InitDebug();
	_InitDevice();
}

Renderer::~Renderer()
{
	delete _window;

	_DeInitInstance();
	_DeInitDebug();
	_DeInitDevice();
}



VkDevice Renderer::GetDevice()
{
	return _device;
}

VkPhysicalDevice Renderer::GetGPU()
{
	return _gpu;
}

VkInstance Renderer::GetInstance()
{
	return _instance;
}

uint32_t Renderer::GetGraphicsFamilyIndex()
{
	return _graphics_family_index;
}

uint32_t Renderer::GetComputeFamilyIndex()
{
	return _compute_family_index;
}

VkQueue  Renderer::GetQueue()
{
	return _queue;
}

VkQueue  Renderer::GetComputeQueue()
{
	return _compute_queue;
}

VkPhysicalDeviceProperties Renderer::GetGPUProperties()
{
	return _gpu_properties;
}

uint32_t Renderer::GetGPUMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32 *memTypeFound)
{
	// NOTICE: android requires different properties.

	for (uint32_t i = 0; i < _memory_properties.memoryTypeCount; i++)
	{
		if ((typeBits & 1) == 1)
		{
			if ((_memory_properties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				if (memTypeFound)
				{
					*memTypeFound = true;
				}
				return i;
			}
		}
		typeBits >>= 1;
	}

	if (memTypeFound)
	{
		*memTypeFound = false;
		return 0;
	}
	else
	{
		throw std::runtime_error("Could not find a matching memory type");
	}
}

Window * Renderer::GetWindow()
{
	return _window;
}


void Renderer::_InitInstance()
{
	
	// set vulkan type, can only be vk_structure_type..
	// set vulkan api version which driver has to support
	VkApplicationInfo application_info {};
	application_info.sType							= VK_STRUCTURE_TYPE_APPLICATION_INFO;
	application_info.apiVersion						= VK_MAKE_VERSION( 1, 0, 26 ); // 1.0.26 newest
	application_info.applicationVersion				= VK_MAKE_VERSION( 0, 0, 50 );
	application_info.pApplicationName				= "Vulkan Engine 0.05";

	// default vk create info, with sType as is, others not to be used.
	// requires application info for amd drivers. - amd beta driver bugg, docs say its optional.
	// otherwise create instance fails.
	VkInstanceCreateInfo instance_create_info {};
	instance_create_info.sType						= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instance_create_info.pApplicationInfo			= &application_info;

	// layers & extensions for debugging.
	instance_create_info.enabledLayerCount			= (uint32_t)_instance_layers.size();
	instance_create_info.ppEnabledLayerNames		= _instance_layers.data();
	instance_create_info.enabledExtensionCount		= (uint32_t)_instance_extensions.size();
	instance_create_info.ppEnabledExtensionNames	= _instance_extensions.data();

	// initialize instance with create info
	// automatic memory managment
	// return instance - last param
	ErrorCheck( vkCreateInstance( &instance_create_info, nullptr, &_instance), "Failed initializing vulkan instance.", "Vulkan v1.0.5 instance initialized." );
}

void Renderer::_DeInitInstance()
{
	// we pass our vulkan instance
	// and again we want automatic memory managment = nullptr
	vkDestroyInstance( _instance, nullptr );

	// make sure instance actually points to nothing
	_instance = nullptr;
}

void Renderer::_InitDevice()
{
	// extract a list of gpus and automatically select one
	{
		// extract gpus info
		uint32_t gpu_count = 0;
		vkEnumeratePhysicalDevices(_instance, &gpu_count, nullptr);
		std::vector<VkPhysicalDevice> gpuList(gpu_count);
		vkEnumeratePhysicalDevices(_instance, &gpu_count, gpuList.data());
		std::cout << " - Available GPU's: " << gpu_count << std::endl;

		// check if gpu exists
		if ( gpu_count == 0 )
			assert(-1 && "Vulkan ERROR: gpu not found.");

		// assing first found gpu
		_gpu = gpuList[0];

		// extract gpu properties
		// can be used to exctract gpu name, vendor etc.
		vkGetPhysicalDeviceProperties( _gpu, &_gpu_properties );
		vkGetPhysicalDeviceMemoryProperties( _gpu, &_memory_properties );
		std::cout << " - Selected GPU Name: " << _gpu_properties.deviceName << std::endl;
	}

	// extract gpu queues and families
	{
		// extract supported queue families from the gpu
		uint32_t family_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties( _gpu, &family_count, nullptr );
		std::cout << " - Selected GPU Available Queues: " << family_count << std::endl;

		std::vector<VkQueueFamilyProperties> family_property_list( family_count );
		vkGetPhysicalDeviceQueueFamilyProperties( _gpu, &family_count, family_property_list.data() );

		// search for an available graphics family properties
		bool found = false;
		int foundCount = 0;
		for ( uint32_t i = 0; i < family_count; ++i ) {
			if ( family_property_list[ i ].queueFlags & VK_QUEUE_GRAPHICS_BIT )	{
				found = true;
				foundCount++;
				_graphics_family_index = i;
			}
		}

		std::cout << " - Selected GPU Available Graphic Bit Queues: " << foundCount << std::endl;

		if (!found) {
			assert( 1 && "Vulkan ERROR: queue family supporting graphics not found." );
			std::exit( -1 );
		}

		// search for an available compute family properties
		found = false;
		foundCount = 0;
		for (uint32_t i = 0; i < family_count; ++i) {
			if (family_property_list[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
				found = true;
				foundCount++;
				_compute_family_index = i;
			}
		}

		std::cout << " - Selected GPU Available Compute Bit Queues: " << foundCount << std::endl;

		if (!found) {
			assert(1 && "Vulkan ERROR: queue family supporting compute not found.");
			std::exit(-1);
		}
	}

	// extract available instance layers
	// allows for debugging
	{
		
		uint32_t layer_count = 0;
		vkEnumerateInstanceLayerProperties( &layer_count, nullptr );
		std::vector<VkLayerProperties> layer_property_list( layer_count );
		vkEnumerateInstanceLayerProperties( &layer_count, layer_property_list.data() );
		
		// prints available instance layers
		std::cout << std::endl;
		std::cout << "Instance Layers: \n";
		for (auto &i : layer_property_list)
			std::cout << "" << i.layerName << "\t\t | " << i.description << std::endl;
		std::cout << std::endl;
	}

	// extract device layer properties
	// allows for debugging
	{
		// extract device layer properties
		uint32_t layer_count = 0;
		vkEnumerateDeviceLayerProperties( _gpu, &layer_count, nullptr );
		std::vector<VkLayerProperties> layer_property_list(layer_count);
		vkEnumerateDeviceLayerProperties( _gpu, &layer_count, layer_property_list.data() );

		// prints available device layers
		std::cout << std::endl;
		std::cout << "Device Layers: \n";
		for (auto &i : layer_property_list)
			std::cout << "" << i.layerName << "\t\t | " << i.description << std::endl;
		std::cout << std::endl;
	}
	
	// create device queue info structure
	// setup queue families how much and what index type. Can be compute, etc..
	// set priorities to queues if both have work.
	float queue_priorities[] { 1.0f };
	float queue_priorities2[]{ 0.9f };
	VkDeviceQueueCreateInfo device_queues[2];

	VkDeviceQueueCreateInfo graphics_queue_create_info {};
	graphics_queue_create_info.sType			= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	graphics_queue_create_info.queueFamilyIndex	= _graphics_family_index;
	graphics_queue_create_info.queueCount		= 1;
	graphics_queue_create_info.pQueuePriorities	= queue_priorities;
	device_queues[0] = graphics_queue_create_info;

	VkDeviceQueueCreateInfo compute_queue_create_info{};
	compute_queue_create_info.sType				= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	compute_queue_create_info.queueFamilyIndex	= _compute_family_index;
	compute_queue_create_info.queueCount		= 1;
	compute_queue_create_info.pQueuePriorities	= queue_priorities2;
	device_queues[1] = compute_queue_create_info;

	// create device info structure
	VkDeviceCreateInfo device_create_info {};
	device_create_info.sType					= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_create_info.queueCreateInfoCount		= 2;
	device_create_info.pQueueCreateInfos		= device_queues;

	// layers & extensions for debugging.
	device_create_info.enabledLayerCount		= (uint32_t)_device_layers.size();
	device_create_info.ppEnabledLayerNames		= _device_layers.data();
	device_create_info.enabledExtensionCount	= (uint32_t)_device_extensions.size();
	device_create_info.ppEnabledExtensionNames	= _device_extensions.data();

	// assign gpu to vulkan device.
	ErrorCheck( vkCreateDevice( _gpu, &device_create_info, nullptr, &_device), "Failed initializing vulkan device.", "Vulkan device initialized." );

	// get both queues.
	vkGetDeviceQueue( _device, _graphics_family_index, 0, &_queue );
	vkGetDeviceQueue( _device, _compute_family_index, 0, &_compute_queue );
}

void Renderer::_DeInitDevice()
{
	// automatically manage memory while destroying the device.
	// returns no errors.
	vkDestroyDevice( _device, nullptr );
}

void Renderer::_SetupDebug()
{
	// use all standard layers
	_instance_layers.push_back("VK_LAYER_LUNARG_standard_validation");
	_device_layers.push_back("VK_LAYER_LUNARG_standard_validation");


	// use specific layers, since we don't need all.
	// if neeeded ones are missing, will crash.
	_instance_layers.push_back( "VK_LAYER_GOOGLE_threading" );
	//_instance_layers.push_back( "VK_LAYER_LUNARG_draw_state" );
	_instance_layers.push_back("VK_LAYER_LUNARG_core_validation");
	_instance_layers.push_back( "VK_LAYER_LUNARG_image" );
	//_instance_layers.push_back( "VK_LAYER_LUNARG_mem_tracker" );
	_instance_layers.push_back( "VK_LAYER_LUNARG_object_tracker" );
	//_instance_layers.push_back( "VK_LAYER_LUNARG_param_checker" );
	_instance_layers.push_back( "VK_LAYER_LUNARG_swapchain" );
	

	// requires an additional extension
	_instance_extensions.push_back( VK_EXT_DEBUG_REPORT_EXTENSION_NAME );
	
	// these are optional, won't crash without them.
	_device_layers.push_back( "VK_LAYER_GOOGLE_threading" );
	//_device_layers.push_back( "VK_LAYER_LUNARG_draw_state" );
	_device_layers.push_back( "VK_LAYER_LUNARG_core_validation" );
	_device_layers.push_back( "VK_LAYER_LUNARG_image" );
	//_device_layers.push_back( "VK_LAYER_LUNARG_mem_tracker" );
	_device_layers.push_back( "VK_LAYER_LUNARG_object_tracker" );
	//_device_layers.push_back( "VK_LAYER_LUNARG_param_checker" );
}

// debug callback.
PFN_vkCreateDebugReportCallbackEXT			fvkCreateDebugReportCallbackEXT				= nullptr;
PFN_vkDestroyDebugReportCallbackEXT			fvkDestroyDebugReportCallbackEXT			= nullptr;

VKAPI_ATTR VkBool32 VKAPI_CALL
VulkanDebugCallback(
	VkDebugReportFlagsEXT		flags,			// what kind of error we are handling here, warning/error etc.
	VkDebugReportObjectTypeEXT	obj_type,		// object type that created an error
	uint64_t					src_object,		
	size_t						location,		// UNSURE: source code line where it happened in the layer.
	int32_t						msg_code,		// related to flags
	const char *				layer_prefix,   // which layer called this
	const char *				msg,			// string of error, human readable
	void *						user_data
	)
{
	// print added callbacks
	std::ostringstream stream;
	stream << "VKDBG: ";
	if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {
		stream << "INFO: ";
	}
	if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
		stream << "WARNING: ";
	}
	if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
		stream << "PERFORMANCE: ";
	}
	if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
		stream << "ERROR: ";
	}
	if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) {
		stream << "DEBUG: ";
	}
	stream << "@[" << layer_prefix << "]: ";
	stream << msg << std::endl;
	std::cout << stream.str();

	// return true will kill the vulkan call.
	// return false will try to finish
	// highly recommended to return false - so it works the same in final product, where debugging is disabled.
	return false;
}

void Renderer::_InitDebug()
{
	fvkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(_instance, "vkCreateDebugReportCallbackEXT");
	fvkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(_instance, "vkDestroyDebugReportCallbackEXT");

	if (fvkCreateDebugReportCallbackEXT == nullptr || fvkDestroyDebugReportCallbackEXT == nullptr) {
		assert(1 && "Vulkan ERROR: Can't fetch debug function pointerss");
		std::exit(-1);
	}
	else {
		std::cout << "Debugger callbacks initialized." << std::endl;
	}

	VkDebugReportCallbackCreateInfoEXT debug_callback_create_info {};
	debug_callback_create_info.sType				= VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
	debug_callback_create_info.pfnCallback			= VulkanDebugCallback;
	debug_callback_create_info.flags				= 
		//VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
		VK_DEBUG_REPORT_WARNING_BIT_EXT |
		//VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
		VK_DEBUG_REPORT_ERROR_BIT_EXT |
		//VK_DEBUG_REPORT_DEBUG_BIT_EXT |
		0;
	
	fvkCreateDebugReportCallbackEXT( _instance, &debug_callback_create_info, nullptr, &_debug_report );
}

void Renderer::_DeInitDebug()
{
	fvkDestroyDebugReportCallbackEXT( _instance, _debug_report, nullptr );
	_debug_report = nullptr;
}

void Renderer::_SetupLayersAndExtensions()
{
	// if you use an operating system, you don't give it directly.
	//_instance_extensions.push_back( VK_KHR_DISPLAY_EXTENSION_NAME );   - not available to most pcs and phones. - renders directly to the screen.
	_instance_extensions.push_back( VK_KHR_SURFACE_EXTENSION_NAME );
	_instance_extensions.push_back( VK_KHR_WIN32_SURFACE_EXTENSION_NAME );
	
	_device_extensions.push_back( VK_KHR_SWAPCHAIN_EXTENSION_NAME );
}

Window * Renderer::OpenWindow(uint32_t size_x, uint32_t size_y, std::string name)
{
	_window = new Window( this, size_x, size_y, name );
	return		_window;
}

bool Renderer::Run()
{
	if (nullptr != _window)
		return _window->Update();
	return true;
}