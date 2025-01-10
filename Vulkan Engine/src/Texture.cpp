#include "Texture.h"
#include <fstream>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb-master\stb-master\stb_image.h>

Texture::Texture(Renderer * renderer, uint32_t width, uint32_t height, VkFormat format, std::vector<char> texture_data, VkImageAspectFlagBits aspectMask)
{
	_CreateImage(renderer, width, height, format);
	_CreateImageMemory(renderer);
	_CreateImageView(renderer, format, aspectMask);
	_CreateSampler(renderer);
	_CreateImageDescriptor();

	/*if (texture_data.size() == 0)	return;
	_CopyTextureData(renderer, texture_data, width, height);
	*/
}

Texture::~Texture()
{
	// destroy image
	// destroy image view.
}



Texture * Texture::Load(Renderer * renderer, std::string file_name)
{
	// get binary file contents
	std::ifstream file(file_name, std::ios::binary);
	if (file.fail()) {
		std::cout << "Could not open \"" << file_name << "\" file!" << std::endl;
		return nullptr;
	}

	std::streampos begin, end;
	begin = file.tellg();
	file.seekg(0, std::ios::end);
	end = file.tellg();

	std::vector<char> file_data(static_cast<size_t>(end - begin));
	file.seekg(0, std::ios::beg);
	file.read(&file_data[0], end - begin);
	file.close();

	// get image data.
	int width = 0, height = 0, components = 0, requested_components = 4;
	unsigned char *image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(&file_data[0]), static_cast<int>(file_data.size()), &width, &height, &components, requested_components);

	if ((image_data == nullptr) ||
		(width <= 0) ||
		(height <= 0) ||
		(components <= 0)) {
		std::cout << "Could not read image data!" << std::endl;
		return nullptr;
	}

	// copy memory into texture_data
	int size = (width)* (height)* (requested_components <= 0 ? components : requested_components);
	std::vector<char> texture_data(size);
	memcpy(&texture_data[0], image_data, size);
	stbi_image_free(image_data);

	// return new texture
	return new Texture(renderer, width, height, VK_FORMAT_R8G8B8A8_UNORM, texture_data);
}

void Texture::Clear(VkClearColorValue color)
{

}



VkDescriptorImageInfo Texture::GetDescriptor()
{
	return _descriptor;
}



void Texture::_CreateImage(Renderer * renderer, uint32_t width, uint32_t height, VkFormat format)
{
	VkImageCreateInfo image_create_info = {};

	image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_create_info.arrayLayers = 1;
	image_create_info.extent.width = width;
	image_create_info.extent.height = height;
	image_create_info.flags = VK_IMAGE_ASPECT_COLOR_BIT;
	image_create_info.format = format;
	image_create_info.imageType = VK_IMAGE_TYPE_2D;
	image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	image_create_info.mipLevels = 1;
	image_create_info.pNext = nullptr;
	image_create_info.pQueueFamilyIndices = nullptr;
	image_create_info.queueFamilyIndexCount = 0;
	image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
	image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	image_create_info.tiling = VK_IMAGE_TILING_LINEAR;
	image_create_info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;

	ErrorCheck( vkCreateImage(renderer->GetDevice(), &image_create_info, nullptr, &_image) );
}

void Texture::_CreateImageView(Renderer * renderer, VkFormat format, VkImageAspectFlagBits aspectMask)
{
	VkImageViewCreateInfo create_info{};
	
	create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	create_info.image = _image;
	create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;			// can be array 3d etc.
	create_info.format = format;
	create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;	// map channel to the shader channel
	create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;    // map channel to the shader channel
	create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;    // map channel to the shader channel
	create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;    // map channel to the shader channel
	create_info.subresourceRange.aspectMask = aspectMask;		// can be depth
	create_info.subresourceRange.baseMipLevel = 0;	// first accessed
	create_info.subresourceRange.levelCount = 1;	// amount of mimap levels. 0 - no image data.
	create_info.subresourceRange.baseArrayLayer = 0;	// 
	create_info.subresourceRange.layerCount = 1;	// 0 - no image data. if view type == array, then we need the amount of those images as layer count.
	
	ErrorCheck( vkCreateImageView(renderer->GetDevice(), &create_info, nullptr, &_image_view) );
}

void Texture::_CreateImageMemory( Renderer * renderer )
{
	// allocate memory
	VkMemoryRequirements image_memory_requirements;
	vkGetImageMemoryRequirements( renderer->GetDevice(), _image, &image_memory_requirements );

	VkPhysicalDeviceMemoryProperties memory_properties;
	vkGetPhysicalDeviceMemoryProperties( renderer->GetGPU(), &memory_properties );

	for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i) 
	{
		if ((image_memory_requirements.memoryTypeBits & (1 << i)) &&
			(memory_properties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
		{
			VkMemoryAllocateInfo memory_allocate_info = {
				VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,     // VkStructureType                        sType
				nullptr,                                    // const void                            *pNext
				image_memory_requirements.size,             // VkDeviceSize                           allocationSize
				i                                           // uint32_t                               memoryTypeIndex
			};

			vkAllocateMemory( renderer->GetDevice(), &memory_allocate_info, nullptr, &_memory);
		}
	}

	// bind memory
	ErrorCheck( vkBindImageMemory( renderer->GetDevice(), _image, _memory, 0 ) );
}

void Texture::_CreateSampler( Renderer * renderer )
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

	ErrorCheck( vkCreateSampler(renderer->GetDevice(), &sampler_create_info, nullptr, &_sampler) );
}

void Texture::_CreateImageDescriptor( )
{
	VkDescriptorImageInfo image_info_descriptor = {};

	image_info_descriptor.imageLayout	= VK_IMAGE_LAYOUT_GENERAL;
	image_info_descriptor.imageView		= _image_view;
	image_info_descriptor.sampler		= _sampler;

	_descriptor							= image_info_descriptor;
}

void Texture::_CopyTextureData( Renderer * renderer, std::vector<char> texture_data, uint32_t width, uint32_t height )
{
	const size_t data_size = texture_data.size();

	// Prepare data in staging buffer
	void *staging_buffer_memory_pointer;
	ErrorCheck( vkMapMemory(renderer->GetDevice(), _memory, 0, data_size, 0, &staging_buffer_memory_pointer) );
	memcpy(staging_buffer_memory_pointer, &texture_data[0], data_size);

	VkMappedMemoryRange flush_range = {
		VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,              // VkStructureType                        sType
		nullptr,                                            // const void                            *pNext
		_memory,                        // VkDeviceMemory                         memory
		0,                                                  // VkDeviceSize                           offset
		data_size                                           // VkDeviceSize                           size
	};
	vkFlushMappedMemoryRanges(renderer->GetDevice(), 1, &flush_range);

	vkUnmapMemory(renderer->GetDevice(), _memory);

	// Prepare command buffer to copy data from staging buffer to a vertex buffer
	VkCommandBufferBeginInfo command_buffer_begin_info = {
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,        // VkStructureType                        sType
		nullptr,                                            // const void                            *pNext
		VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,        // VkCommandBufferUsageFlags              flags
		nullptr                                             // const VkCommandBufferInheritanceInfo  *pInheritanceInfo
	};

	/*VkCommandBuffer command_buffer = Vulkan.RenderingResources[0].CommandBuffer;

	vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);

	VkImageSubresourceRange image_subresource_range = {
		VK_IMAGE_ASPECT_COLOR_BIT,                          // VkImageAspectFlags                     aspectMask
		0,                                                  // uint32_t                               baseMipLevel
		1,                                                  // uint32_t                               levelCount
		0,                                                  // uint32_t                               baseArrayLayer
		1                                                   // uint32_t                               layerCount
	};

	VkImageMemoryBarrier image_memory_barrier_from_undefined_to_transfer_dst = {
		VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,             // VkStructureType                        sType
		nullptr,                                            // const void                            *pNext
		0,                                                  // VkAccessFlags                          srcAccessMask
		VK_ACCESS_TRANSFER_WRITE_BIT,                       // VkAccessFlags                          dstAccessMask
		VK_IMAGE_LAYOUT_UNDEFINED,                          // VkImageLayout                          oldLayout
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,               // VkImageLayout                          newLayout
		VK_QUEUE_FAMILY_IGNORED,                            // uint32_t                               srcQueueFamilyIndex
		VK_QUEUE_FAMILY_IGNORED,                            // uint32_t                               dstQueueFamilyIndex
		_image,											    // VkImage                                image
		image_subresource_range                             // VkImageSubresourceRange                subresourceRange
	};
	vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &image_memory_barrier_from_undefined_to_transfer_dst);

	/*VkBufferImageCopy buffer_image_copy_info = {
		0,                                                  // VkDeviceSize                           bufferOffset
		0,                                                  // uint32_t                               bufferRowLength
		0,                                                  // uint32_t                               bufferImageHeight
		{                                                   // VkImageSubresourceLayers               imageSubresource
			VK_IMAGE_ASPECT_COLOR_BIT,                          // VkImageAspectFlags                     aspectMask
			0,                                                  // uint32_t                               mipLevel
		0,                                                  // uint32_t                               baseArrayLayer
		1                                                   // uint32_t                               layerCount
		},
		{                                                   // VkOffset3D                             imageOffset
			0,                                                  // int32_t                                x
			0,                                                  // int32_t                                y
		0                                                   // int32_t                                z
		},
		{                                                   // VkExtent3D                             imageExtent
			width,                                              // uint32_t                               width
			height,                                             // uint32_t                               height
		1                                                   // uint32_t                               depth
		}
	};
	vkCmdCopyBufferToImage(command_buffer, Vulkan.StagingBuffer.Handle, Vulkan.Image.Handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &buffer_image_copy_info);

	VkImageMemoryBarrier image_memory_barrier_from_transfer_to_shader_read = {
		VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,             // VkStructureType                        sType
		nullptr,                                            // const void                            *pNext
		VK_ACCESS_TRANSFER_WRITE_BIT,                       // VkAccessFlags                          srcAccessMask
		VK_ACCESS_SHADER_READ_BIT,                          // VkAccessFlags                          dstAccessMask
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,               // VkImageLayout                          oldLayout
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,           // VkImageLayout                          newLayout
		VK_QUEUE_FAMILY_IGNORED,                            // uint32_t                               srcQueueFamilyIndex
		VK_QUEUE_FAMILY_IGNORED,                            // uint32_t                               dstQueueFamilyIndex
		Vulkan.Image.Handle,                                // VkImage                                image
		image_subresource_range                             // VkImageSubresourceRange                subresourceRange
	};
	vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &image_memory_barrier_from_transfer_to_shader_read);

	vkEndCommandBuffer(command_buffer);

	// Submit command buffer and copy data from staging buffer to a vertex buffer
	VkSubmitInfo submit_info = {
		VK_STRUCTURE_TYPE_SUBMIT_INFO,                      // VkStructureType                        sType
		nullptr,                                            // const void                            *pNext
		0,                                                  // uint32_t                               waitSemaphoreCount
		nullptr,                                            // const VkSemaphore                     *pWaitSemaphores
		nullptr,                                            // const VkPipelineStageFlags            *pWaitDstStageMask;
		1,                                                  // uint32_t                               commandBufferCount
		&command_buffer,                                    // const VkCommandBuffer                 *pCommandBuffers
		0,                                                  // uint32_t                               signalSemaphoreCount
		nullptr                                             // const VkSemaphore                     *pSignalSemaphores
	};

	if (vkQueueSubmit(GetGraphicsQueue().Handle, 1, &submit_info, VK_NULL_HANDLE) != VK_SUCCESS) {
		return false;
	}

	vkDeviceWaitIdle(GetDevice());
	*/
}

/*std::vector<char> Texture::_GetImageContents(std::string file_name)
{
	// get binary file contents
	std::ifstream file(file_name, std::ios::binary);
	if (file.fail()) {
		std::cout << "Could not open \"" << file_name << "\" file!" << std::endl;
		return std::vector<char>();
	}

	std::streampos begin, end;
	begin = file.tellg();
	file.seekg(0, std::ios::end);
	end = file.tellg();

	std::vector<char> file_data(static_cast<size_t>(end - begin));
	file.seekg(0, std::ios::beg);
	file.read(&file_data[0], end - begin);
	file.close();

	// get image data.
	int width = 0, height = 0, components = 0, requested_components = 0;
	unsigned char *image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(&file_data[0]), static_cast<int>(file_data.size()), &width, &height, &components, requested_components);

	if ((image_data == nullptr) ||
		(width <= 0) ||
		(height <= 0) ||
		(components <= 0)) {
			std::cout << "Could not read image data!" << std::endl;
			return std::vector<char>();
	}

	int size = (width)* (height)* (requested_components <= 0 ? components : requested_components);

	std::vector<char> texture_data(size);
	memcpy(&texture_data[0], image_data, size);

	stbi_image_free(image_data);

	return texture_data;
}
*/