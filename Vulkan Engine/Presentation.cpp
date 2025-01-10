#include "Presentation.h"



Presentation::Presentation(Renderer * renderer, VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR surface_capabilities, VkSurfaceFormatKHR format, uint32_t surface_x, uint32_t surface_y)
{
	_renderer					= renderer;

	_surface					= surface;
	_surface_capabilities		= surface_capabilities;
	_surface_format				= format;
	_surface_size_x				= surface_x;
	_surface_size_y             = surface_y;

	std::cout << "-------------------------------------- Creating Presentation SwapChain -----------------------------------" << std::endl;

	_InitSwapChain();
	_InitSwapChainImages();

	_CreateCommandPoolAndBuffers();
	_CreateSemaphores();

	_CreatePresentationSampler();
}

Presentation::~Presentation()
{
	_DeInitSwapChainImages();
	_DeInitSwapChain();
}


uint32_t Presentation::PreviousFrame()
{
	return _previous_image;
}

uint32_t Presentation::PrepareFrame()
{
	_previous_image = _current_image;
	uint32_t image_index;
	ErrorCheck(vkAcquireNextImageKHR(_renderer->GetDevice(), _swapchain, UINT64_MAX, _semaphore_image_available, VK_NULL_HANDLE, &image_index), "Swapchain image aquisition successfull.");
	_current_image = image_index;
	return image_index;
}

void Presentation::RenderFrame(uint32_t image_index)
{
	// present swapchain image to the screen.
	VkPresentInfoKHR present_info = {};

	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.pNext = nullptr;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = &_semaphore_rendering_finished;
	present_info.swapchainCount = 1;
	present_info.pSwapchains = &_swapchain;
	present_info.pImageIndices = &image_index;
	present_info.pResults = nullptr;

	ErrorCheck(vkQueuePresentKHR(_renderer->GetQueue(), &present_info), "Unable to present an image to the screen." "Image presentation was successfull");
}

void Presentation::Clear()
{
	uint32_t image_index = PrepareFrame();

	// submit queue.
	VkPipelineStageFlags wait_dst_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;
	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.pNext = nullptr;
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = &_semaphore_image_available;
	submit_info.pWaitDstStageMask = &wait_dst_stage_mask;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &_present_queue_command_buffers[image_index];
	submit_info.signalSemaphoreCount = 1,
	submit_info.pSignalSemaphores = &_semaphore_rendering_finished;

	ErrorCheck(vkQueueSubmit(_renderer->GetQueue(), 1, &submit_info, VK_NULL_HANDLE), "Unable to submit the queue.", "Submitting the queue was successfull");

	RenderFrame(image_index);
}



VkDescriptorImageInfo Presentation::GetPresentationImageDescriptor(uint32_t image_index)
{
	VkDescriptorImageInfo image_info_descriptor = {};

	image_info_descriptor.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	image_info_descriptor.imageView = _swapchain_image_views[image_index];
	image_info_descriptor.sampler = _sampler;

	return image_info_descriptor;
}

VkSampler Presentation::GetPresentationSampler()
{
	return _sampler;
}

std::vector<VkImage> Presentation::GetSwapchainImages()
{
	return _swapchain_images;
}

std::vector<VkImageView> Presentation::GetSwapchainImageViews()
{
	return _swapchain_image_views;
}

VkSemaphore & Presentation::GetSemaphoreRenderingFinished()
{
	return _semaphore_rendering_finished;
}

VkSemaphore & Presentation::GetSemaphoreImageAvailable()
{
	return _semaphore_image_available;
}



void Presentation::_InitSwapChain()
{
	// prevent too much.
	if (_swapchain_image_count > _surface_capabilities.maxImageCount) _swapchain_image_count = _surface_capabilities.maxImageCount;
	if (_swapchain_image_count < _surface_capabilities.minImageCount + 1) _swapchain_image_count = _surface_capabilities.minImageCount + 1;

	VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR; // select mode, double buffering

															  // define present modes
	{
		// extract present modes.
		uint32_t present_mode_count = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(_renderer->GetGPU(), _surface, &present_mode_count, nullptr);
		std::vector<VkPresentModeKHR> present_mode_list(present_mode_count);
		vkGetPhysicalDeviceSurfacePresentModesKHR(_renderer->GetGPU(), _surface, &present_mode_count, present_mode_list.data());

		// double sync, triple sync etc.
		for (auto m : present_mode_list)
		{
			if (m == VK_PRESENT_MODE_MAILBOX_KHR) present_mode = m;		// newer devices
			else if (m == VK_PRESENT_MODE_FIFO_KHR) present_mode = m;	// old devices
			else if (m == VK_PRESENT_MODE_IMMEDIATE_KHR) present_mode = m;	// way out of the league old - or if tearing doesn't matter to one
		}
	}

	VkSwapchainCreateInfoKHR swapchain_create_info{};
	swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchain_create_info.surface = _surface;
	swapchain_create_info.minImageCount = _swapchain_image_count;					// we want atleast 2 images - double buffering. possibly can create triple bufferimg.
	swapchain_create_info.imageFormat = _surface_format.format;
	swapchain_create_info.imageColorSpace = _surface_format.colorSpace;
	swapchain_create_info.imageExtent.width = _surface_size_x;
	swapchain_create_info.imageExtent.height = _surface_size_y;
	swapchain_create_info.imageArrayLayers = 1;										// 1 = regular, 2 = stereoscopic rendering, can set more for three eyed people n more.
	swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
		VK_IMAGE_USAGE_SAMPLED_BIT |
		VK_IMAGE_USAGE_STORAGE_BIT;		// what we gonna do with the image, in our case we want to render to it.

	swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;				// we dont want to share them between queue families, but if we want we can reusse them per multiple gpus
	swapchain_create_info.queueFamilyIndexCount = 0;
	swapchain_create_info.pQueueFamilyIndices = nullptr;
	swapchain_create_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;	// rotate and stuff
	swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;		// can be used to display double windows on top of each other in certain OS
	swapchain_create_info.presentMode = present_mode;								// vsync option
	swapchain_create_info.clipped = true;										// dont render whats invisibile, might save battery life & proccessing power. pretty much leave enabled.
	swapchain_create_info.oldSwapchain = nullptr;									// useful when resizing window

	ErrorCheck(vkCreateSwapchainKHR(_renderer->GetDevice(), &swapchain_create_info, nullptr, &_swapchain), "Unable to create a presentation surface", "Presentation surface has been created.");

	vkGetSwapchainImagesKHR(_renderer->GetDevice(), _swapchain, &_swapchain_image_count, nullptr);
}

void Presentation::_DeInitSwapChain()
{
	vkDestroySwapchainKHR(_renderer->GetDevice(), _swapchain, nullptr);
}

void Presentation::_InitSwapChainImages()
{
	_swapchain_images.resize(_swapchain_image_count);
	_swapchain_image_views.resize(_swapchain_image_count);

	vkGetSwapchainImagesKHR(_renderer->GetDevice(), _swapchain, &_swapchain_image_count, _swapchain_images.data());

	for (uint32_t i = 0; i < _swapchain_image_count; i++)
	{
		VkImageViewCreateInfo create_info{};

		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		create_info.image = _swapchain_images[i];
		create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;			// can be array 3d etc.
		create_info.format = _surface_format.format;
		create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;	// map channel to the shader channel
		create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;    // map channel to the shader channel
		create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;    // map channel to the shader channel
		create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;    // map channel to the shader channel
		create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;		// can be depth
		create_info.subresourceRange.baseMipLevel = 0;	// first accessed
		create_info.subresourceRange.levelCount = 1;	// amount of mimap levels. 0 - no image data.
		create_info.subresourceRange.baseArrayLayer = 0;	// 
		create_info.subresourceRange.layerCount = 1;	// 0 - no image data. if view type == array, then we need the amount of those images as layer count.

		vkCreateImageView(_renderer->GetDevice(), &create_info, nullptr, &_swapchain_image_views[i]);
	}
}

void Presentation::_DeInitSwapChainImages()
{
	for (uint32_t i = 0; i < _swapchain_image_count; i++)
	{
		vkDestroyImageView(_renderer->GetDevice(), _swapchain_image_views[i], nullptr);
	}
}


void Presentation::_CreateSemaphores()
{
	ErrorCheck( vkCreateSemaphore(_renderer->GetDevice(), &Structs::SemaphoreCreateInfo(), nullptr, &_semaphore_image_available ),
		"Unable to create a semaphore.", "Semaphore image available successfully created.");

	ErrorCheck( vkCreateSemaphore(_renderer->GetDevice(), &Structs::SemaphoreCreateInfo(), nullptr, &_semaphore_rendering_finished ),
		"Unable to create a semaphore.", "Semaphore finished succesfully created.");
}

void Presentation::_CreateCommandPoolAndBuffers()
{
	VkCommandPoolCreateInfo cmd_pool_create_info = {};
	cmd_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmd_pool_create_info.pNext = nullptr;
	cmd_pool_create_info.flags = 0;
	cmd_pool_create_info.queueFamilyIndex = _renderer->GetGraphicsFamilyIndex();
	ErrorCheck(vkCreateCommandPool(_renderer->GetDevice(), &cmd_pool_create_info, nullptr, &_present_queue_command_pool), "Unable to create a command pool.", "Presentation queue command pool successfully created.");

	_present_queue_command_buffers.resize(_swapchain_image_count);



	VkCommandBufferAllocateInfo cmd_buffer_allocate_info = {};
	cmd_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmd_buffer_allocate_info.pNext = nullptr;
	cmd_buffer_allocate_info.commandPool = _present_queue_command_pool;
	cmd_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmd_buffer_allocate_info.commandBufferCount = _swapchain_image_count;

	ErrorCheck(vkAllocateCommandBuffers(_renderer->GetDevice(), &cmd_buffer_allocate_info, &_present_queue_command_buffers[0]), "Unable to allocated command buffers.", "Command buffers have been allocated.");

	_RecordCommandBuffers();
}

void Presentation::_RecordCommandBuffers()
{
	VkCommandBufferBeginInfo cmd_buffer_begin_info = {};
	cmd_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmd_buffer_begin_info.pNext = nullptr;
	cmd_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	cmd_buffer_begin_info.pInheritanceInfo = nullptr;

	VkClearColorValue clear_color = {
		{ 0.0f, 0.0f, 0.0f, 0.0f }
	};

	VkImageSubresourceRange image_subresource_range = {
		VK_IMAGE_ASPECT_COLOR_BIT,                    // VkImageAspectFlags                     aspectMask
		0,                                            // uint32_t                               baseMipLevel
		1,                                            // uint32_t                               levelCount
		0,                                            // uint32_t                               baseArrayLayer
		1                                             // uint32_t                               layerCount
	};

	for (uint32_t i = 0; i < _swapchain_image_count; ++i) {

		VkImageMemoryBarrier barrier_from_present_to_clear = {
			VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,     // VkStructureType                        sType
			nullptr,                                    // const void                            *pNext
			VK_ACCESS_MEMORY_READ_BIT,                  // VkAccessFlags                          srcAccessMask
			VK_ACCESS_TRANSFER_WRITE_BIT,               // VkAccessFlags                          dstAccessMask
			VK_IMAGE_LAYOUT_UNDEFINED,                    // VkImageLayout                          oldLayout
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,       // VkImageLayout                          newLayout
			VK_QUEUE_FAMILY_IGNORED,                    // uint32_t                               srcQueueFamilyIndex
			VK_QUEUE_FAMILY_IGNORED,                    // uint32_t                               dstQueueFamilyIndex
			_swapchain_images[i],                       // VkImage                                image
			image_subresource_range                     // VkImageSubresourceRange                subresourceRange
		};

		VkImageMemoryBarrier barrier_from_clear_to_present = {
			VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,     // VkStructureType                        sType
			nullptr,                                    // const void                            *pNext
			VK_ACCESS_TRANSFER_WRITE_BIT,               // VkAccessFlags                          srcAccessMask
			VK_ACCESS_MEMORY_READ_BIT,                  // VkAccessFlags                          dstAccessMask
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,       // VkImageLayout                          oldLayout
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,            // VkImageLayout                          newLayout
			VK_QUEUE_FAMILY_IGNORED,                    // uint32_t                               srcQueueFamilyIndex
			VK_QUEUE_FAMILY_IGNORED,                    // uint32_t                               dstQueueFamilyIndex
			_swapchain_images[i],                       // VkImage                                image
			image_subresource_range                     // VkImageSubresourceRange                subresourceRange
		};

		vkBeginCommandBuffer(_present_queue_command_buffers[i], &cmd_buffer_begin_info);

		vkCmdPipelineBarrier(_present_queue_command_buffers[i], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier_from_present_to_clear);

		vkCmdClearColorImage(_present_queue_command_buffers[i], _swapchain_images[i], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clear_color, 1, &image_subresource_range);

		vkCmdPipelineBarrier(_present_queue_command_buffers[i], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier_from_clear_to_present);

		ErrorCheck(vkEndCommandBuffer(_present_queue_command_buffers[i]), "Unable to record a command buffer", "Command buffer recorded succesfully");
	}
}

void Presentation::_CreatePresentationSampler()
{
	VkSamplerCreateInfo sampler_create_info = {
		VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,                // VkStructureType            sType
		nullptr,                                              // const void*                pNext
		0,                                                    // VkSamplerCreateFlags       flags
		VK_FILTER_LINEAR,                                     // VkFilter                   magFilter
		VK_FILTER_LINEAR,                                     // VkFilter                   minFilter
		VK_SAMPLER_MIPMAP_MODE_NEAREST,                       // VkSamplerMipmapMode        mipmapMode
		VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,                // VkSamplerAddressMode       addressModeU
		VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,                // VkSamplerAddressMode       addressModeV
		VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,                // VkSamplerAddressMode       addressModeW
		0.0f,                                                 // float                      mipLodBias
		VK_FALSE,                                             // VkBool32                   anisotropyEnable
		1.0f,                                                 // float                      maxAnisotropy
		VK_FALSE,                                             // VkBool32                   compareEnable
		VK_COMPARE_OP_ALWAYS,                                 // VkCompareOp                compareOp
		0.0f,                                                 // float                      minLod
		0.0f,                                                 // float                      maxLod
		VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,              // VkBorderColor              borderColor
		VK_FALSE                                              // VkBool32                   unnormalizedCoordinates
	};

	ErrorCheck(vkCreateSampler(_renderer->GetDevice(), &sampler_create_info, nullptr, &_sampler), "Unable to create presentation image sampler.", "Presentation image sampler created.");
}


