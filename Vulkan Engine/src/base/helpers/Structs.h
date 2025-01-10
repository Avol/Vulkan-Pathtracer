#pragma once

#include "../../Platform.h"
#include <vector>

class Structs
{
	public:
		static VkDescriptorSetLayoutBinding	 		DescriptorSetLayoutBinding(VkDescriptorType type, VkShaderStageFlags flags, uint32_t binding);
		static VkDescriptorSetLayoutCreateInfo &	DescriptorSetLayoutCreateInfo(std::vector<VkDescriptorSetLayoutBinding> & bindings);

		static VkDescriptorPoolSize					DescriptorPoolSize(VkDescriptorType type, uint32_t descriptorCount);
		static VkDescriptorPoolCreateInfo			DescriptorPoolCreateInfo(std::vector<VkDescriptorPoolSize> & pool_sizes);

		static VkWriteDescriptorSet					WriteDescriptorSet(VkDescriptorSet dstSet, VkDescriptorType type, uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
		static VkWriteDescriptorSet					WriteDescriptorSet(VkDescriptorSet dstSet, VkDescriptorType type, uint32_t binding, VkDescriptorImageInfo* imageInfo);
		static VkDescriptorSetAllocateInfo			DescriptorSetAllocateInfo(VkDescriptorPool pool, VkDescriptorSetLayout & set_layout);

		static VkDescriptorImageInfo				DescriptorImageInfo(VkImageView image, VkSampler sampler);
		

		static VkPipelineLayoutCreateInfo			PipelineLayoutCreateInfo(VkDescriptorSetLayout & descript_set_layout);
		static VkPipelineCacheCreateInfo			PipelineCacheCreateInfo();
		static VkComputePipelineCreateInfo			ComputePipelineCreateInfo(VkPipelineLayout pipeline_layout);

		static VkCommandPoolCreateInfo				CommandPoolCreateInfo(uint32_t compute_family_index);
		static VkCommandBufferAllocateInfo			CommandBufferAllocateInfo(VkCommandPool pool, uint32_t buffer_count);
		static VkCommandBufferBeginInfo				CommandBufferBeginInfo();

		static VkImageSubresourceRange				ImageSubresourceRange(VkImageAspectFlags aspectMask);


		static VkSubmitInfo							SubmitInfo(VkCommandBuffer & command_buffer, VkSemaphore & availabe, VkSemaphore & finished, VkPipelineStageFlags wait_dst_stage_mask);

		static VkPipelineShaderStageCreateInfo		PipelineShaderStageCreateInfo(const char* name, VkShaderStageFlagBits & stage, VkShaderModule & module);

		static VkDescriptorBufferInfo				DescriptorBufferInfo(VkBuffer & buffer, VkDeviceSize size);

		static VkMemoryAllocateInfo					MemoryAllocateInfo();

		static VkBufferCreateInfo					BufferCreateInfo(VkBufferUsageFlags usage_flags, VkDeviceSize size);

		static VkFenceCreateInfo					FenceCreateInfo();
		static VkSemaphoreCreateInfo				SemaphoreCreateInfo();
};

