#pragma once

#include "src\Platform.h"
#include "src\Shared.h"
#include "src\Renderer.h"
#include "src\base\helpers\Structs.h"

#include <vector>

class Renderer;
class Presentation
{
private:
	VkSurfaceKHR						_surface;
	VkSurfaceCapabilitiesKHR			_surface_capabilities;
	VkSurfaceFormatKHR					_surface_format;
	uint32_t							_surface_size_x;
	uint32_t							_surface_size_y;

	VkCommandPool						_present_queue_command_pool;
	std::vector<VkCommandBuffer>		_present_queue_command_buffers;

	VkSwapchainKHR						_swapchain										= VK_NULL_HANDLE;
	uint32_t							_swapchain_image_count							= 2;
	std::vector<VkImage>				_swapchain_images;
	std::vector<VkImageView>			_swapchain_image_views;
	VkSampler							_sampler;

	VkSemaphore							_semaphore_image_available						= VK_NULL_HANDLE;
	VkSemaphore							_semaphore_rendering_finished					= VK_NULL_HANDLE;

	uint32_t                            _current_image                                  = 0;
	uint32_t                            _previous_image                                 = 0;

public:
	Presentation(Renderer * renderer, VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR surface_capabilities, VkSurfaceFormatKHR format, uint32_t surface_x, uint32_t surface_y);
	~Presentation();

	uint32_t                            PreviousFrame();
	uint32_t							PrepareFrame();
	void								RenderFrame(uint32_t image_index);
	void								Clear();

	VkDescriptorImageInfo				GetPresentationImageDescriptor(uint32_t image_index);
	std::vector<VkImage>				GetSwapchainImages();
	std::vector<VkImageView>			GetSwapchainImageViews();
	VkSemaphore				&			GetSemaphoreRenderingFinished();
	VkSemaphore				&			GetSemaphoreImageAvailable();
	VkSampler							GetPresentationSampler();

private:
	Renderer				*			_renderer				= nullptr;

	void								_InitSwapChain();
	void								_DeInitSwapChain();

	void								_InitSwapChainImages();
	void								_DeInitSwapChainImages();

	void								_CreateSemaphores();
	void								_CreatePresentationSampler();

	void								_CreateCommandPoolAndBuffers();
	void								_RecordCommandBuffers();

};

