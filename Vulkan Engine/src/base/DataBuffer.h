#pragma once

#include "../Platform.h"
#include "../Shared.h"
#include "../Renderer.h"
#include "helpers\Structs.h"
#include "glm\glm.hpp"

class DataBuffer
{
	private:
		uint32_t                            _buffer_size;
		VkDeviceSize                        _offset;

		VkBuffer							_buffer;
		VkDeviceMemory						_memory;
		VkDescriptorBufferInfo				_descriptor_info;
		VkDescriptorSet                     _descriptor;
		VkWriteDescriptorSet                _write_descriptor;
		void				*				_mapped;
		VkMemoryRequirements				_memory_requirements;
	public:
		enum DataBufferType	{ UNIFORM, SBO };

		DataBuffer( Renderer * renderer, VkBufferUsageFlags usage_flags, void * data, uint32_t size, VkDeviceSize offset = 0);
		~DataBuffer();
		void								Update( Renderer * renderer, void * data );
		VkDescriptorSet                     GetDescriptorSet();
		VkDescriptorBufferInfo        *     GetDescriptorInfo();
};