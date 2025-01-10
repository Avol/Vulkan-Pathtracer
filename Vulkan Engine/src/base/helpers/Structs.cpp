#include "Structs.h"

VkDescriptorSetLayoutBinding Structs::DescriptorSetLayoutBinding(VkDescriptorType type, VkShaderStageFlags flags, uint32_t binding)
{
	VkDescriptorSetLayoutBinding layout = {};

	layout.descriptorType		= type;
	layout.stageFlags			= flags;
	layout.binding				= binding;
	layout.descriptorCount		= 1;

	return layout;
}

VkDescriptorSetLayoutCreateInfo & Structs::DescriptorSetLayoutCreateInfo(std::vector<VkDescriptorSetLayoutBinding> & bindings)
{
	VkDescriptorSetLayoutCreateInfo create_info = {};

	create_info.sType				= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	create_info.pNext				= NULL;
	create_info.pBindings			= bindings.data();
	create_info.bindingCount		= (uint32_t)bindings.size();

	return create_info;
}


VkDescriptorPoolSize Structs::DescriptorPoolSize(VkDescriptorType type, uint32_t descriptorCount)
{
	VkDescriptorPoolSize descriptor_pool_size = {};
	descriptor_pool_size.type = type;
	descriptor_pool_size.descriptorCount = descriptorCount;
	return descriptor_pool_size;
}

VkDescriptorPoolCreateInfo Structs::DescriptorPoolCreateInfo(std::vector<VkDescriptorPoolSize> & pool_sizes)
{
	VkDescriptorPoolCreateInfo pool_info = {};

	pool_info.sType				= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.pNext				= NULL;
	pool_info.poolSizeCount		= (uint32_t)pool_sizes.size();
	pool_info.pPoolSizes		= pool_sizes.data();
	pool_info.maxSets			= 3;

	return pool_info;
}


VkWriteDescriptorSet Structs::WriteDescriptorSet(VkDescriptorSet dstSet, VkDescriptorType type, uint32_t binding, VkDescriptorBufferInfo* bufferInfo)
{
	VkWriteDescriptorSet writeDescriptorSet = {};

	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.pNext = NULL;
	writeDescriptorSet.dstSet = dstSet;
	writeDescriptorSet.descriptorType = type;
	writeDescriptorSet.dstBinding = binding;
	writeDescriptorSet.pBufferInfo = bufferInfo;
	writeDescriptorSet.descriptorCount = 1;
	writeDescriptorSet.dstArrayElement = 0;

	return writeDescriptorSet;
}

VkWriteDescriptorSet Structs::WriteDescriptorSet(VkDescriptorSet dstSet, VkDescriptorType type, uint32_t binding, VkDescriptorImageInfo* imageInfo)
{
	VkWriteDescriptorSet writeDescriptorSet = {};

	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.pNext = NULL;
	writeDescriptorSet.dstSet = dstSet;
	writeDescriptorSet.descriptorType = type;
	writeDescriptorSet.dstBinding = binding;
	writeDescriptorSet.pImageInfo = imageInfo;
	writeDescriptorSet.descriptorCount = 1;

	return writeDescriptorSet;
}

VkDescriptorSetAllocateInfo Structs::DescriptorSetAllocateInfo(VkDescriptorPool pool, VkDescriptorSetLayout & set_layout)
{
	VkDescriptorSetAllocateInfo allocate_info = {};

	allocate_info.sType					= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocate_info.descriptorPool		= pool;
	allocate_info.pSetLayouts			= &set_layout;
	allocate_info.descriptorSetCount	= 1;
	allocate_info.pNext					= nullptr;

	return allocate_info;
}


VkDescriptorImageInfo Structs::DescriptorImageInfo(VkImageView image, VkSampler sampler)
{
	VkDescriptorImageInfo image_info_descriptor = {};

	image_info_descriptor.imageLayout	= VK_IMAGE_LAYOUT_GENERAL;
	image_info_descriptor.imageView		= image;
	image_info_descriptor.sampler		= sampler;

	return image_info_descriptor;
}

VkImageSubresourceRange	Structs::ImageSubresourceRange(VkImageAspectFlags aspectMask)
{
	VkImageSubresourceRange image_subresource_range = {};

	image_subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	image_subresource_range.baseMipLevel = 0;
	image_subresource_range.levelCount = 1;
	image_subresource_range.baseArrayLayer = 0;
	image_subresource_range.layerCount = 1;

	return image_subresource_range;
}


VkPipelineLayoutCreateInfo Structs::PipelineLayoutCreateInfo(VkDescriptorSetLayout & descript_set_layout)
{
	VkPipelineLayoutCreateInfo create_info = {};

	create_info.sType			= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	create_info.pNext			= nullptr;
	create_info.setLayoutCount	= 1;
	create_info.pSetLayouts		= &descript_set_layout;

	return create_info;
}

VkPipelineCacheCreateInfo Structs::PipelineCacheCreateInfo()
{
	VkPipelineCacheCreateInfo create_info = {};

	create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

	return create_info;
}

VkComputePipelineCreateInfo	Structs::ComputePipelineCreateInfo(VkPipelineLayout pipeline_layout)
{
	VkComputePipelineCreateInfo create_info = {};

	create_info.sType				= VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	create_info.layout				= pipeline_layout;
	create_info.flags				= 0;
	create_info.pNext				= nullptr;
	create_info.basePipelineHandle	= nullptr;

	return create_info;
}


VkPipelineShaderStageCreateInfo	Structs::PipelineShaderStageCreateInfo(const char* name, VkShaderStageFlagBits & stage, VkShaderModule & module)
{
	VkPipelineShaderStageCreateInfo shaderStage = {};

	shaderStage.sType		= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStage.stage		= stage;
	shaderStage.module		= module;
	shaderStage.pName		= name;

	return shaderStage;
}


VkCommandPoolCreateInfo	Structs::CommandPoolCreateInfo(uint32_t compute_family_index)
{
	VkCommandPoolCreateInfo create_info = {};

	create_info.sType				= VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	create_info.pNext				= nullptr;
	create_info.flags				= 0;
	create_info.queueFamilyIndex	= compute_family_index;

	return create_info;
}

VkCommandBufferAllocateInfo	Structs::CommandBufferAllocateInfo(VkCommandPool pool, uint32_t buffer_count)
{
	VkCommandBufferAllocateInfo allocate_info = {};

	allocate_info.sType					= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocate_info.pNext					= nullptr;
	allocate_info.commandPool			= pool;
	allocate_info.level					= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocate_info.commandBufferCount	= buffer_count;

	return allocate_info;
}

VkCommandBufferBeginInfo Structs::CommandBufferBeginInfo()
{
	VkCommandBufferBeginInfo begin_info = {};

	begin_info.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.pNext				= nullptr;
	begin_info.flags				= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	begin_info.pInheritanceInfo		= nullptr;

	return begin_info;
}

VkSubmitInfo Structs::SubmitInfo(VkCommandBuffer & command_buffer, VkSemaphore & availabe, VkSemaphore & finished, VkPipelineStageFlags wait_dst_stage_mask)
{
	VkSubmitInfo submit_info = {};

	submit_info.sType					= VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.pNext					= nullptr;
	submit_info.commandBufferCount		= 1;
	submit_info.waitSemaphoreCount		= 1;
	submit_info.pWaitSemaphores			= &availabe;
	submit_info.pWaitDstStageMask		= &wait_dst_stage_mask;
	submit_info.commandBufferCount		= 1;
	submit_info.pCommandBuffers			= &command_buffer;
	submit_info.signalSemaphoreCount	= 1,
	submit_info.pSignalSemaphores		= &finished;

	return submit_info;
}


VkFenceCreateInfo Structs::FenceCreateInfo()
{
	VkFenceCreateInfo fence_create_info = {};

	fence_create_info.sType		= VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_create_info.flags		= VK_FENCE_CREATE_SIGNALED_BIT;

	return fence_create_info;
}

VkDescriptorBufferInfo Structs::DescriptorBufferInfo(VkBuffer & buffer, VkDeviceSize size)
{
	VkDescriptorBufferInfo descriptor_info = {};

	descriptor_info.offset		= 0;
	descriptor_info.buffer		= buffer;
	descriptor_info.range		= size;

	return descriptor_info;
}


VkMemoryAllocateInfo Structs::MemoryAllocateInfo()
{
	VkMemoryAllocateInfo memory_allocation_info = {};

	memory_allocation_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memory_allocation_info.pNext = NULL;

	return memory_allocation_info;
}

VkBufferCreateInfo Structs::BufferCreateInfo(VkBufferUsageFlags usage_flags, VkDeviceSize size)
{
	VkBufferCreateInfo create_info = {};

	create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	create_info.pNext = NULL;
	create_info.size  = size;
	create_info.usage = usage_flags;

	return create_info;
}

VkSemaphoreCreateInfo Structs::SemaphoreCreateInfo()
{
	VkSemaphoreCreateInfo semaphore_create_info = {};

	semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphore_create_info.pNext = nullptr;
	semaphore_create_info.flags = 0;

	return semaphore_create_info;
}