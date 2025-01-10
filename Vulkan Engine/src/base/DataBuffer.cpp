#include "DataBuffer.h"

DataBuffer::DataBuffer( Renderer * renderer, VkBufferUsageFlags usage_flags, void * data, uint32_t buffer_size, VkDeviceSize offset)
{
	_buffer_size = buffer_size;
	_offset      = offset;

	// prerequisities
	VkMemoryPropertyFlags	memory_property_flags	= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	
	// create buffer
	VkBufferCreateInfo		buffer_create_info = Structs::BufferCreateInfo(usage_flags, _buffer_size);
	ErrorCheck( vkCreateBuffer(renderer->GetDevice(), &buffer_create_info, nullptr, &_buffer), "Unable to create buffer");
	
	// get memory requirements
	vkGetBufferMemoryRequirements(renderer->GetDevice(), *&_buffer, &_memory_requirements);

	// allocate memory
	VkMemoryAllocateInfo	memory_allocation_info = Structs::MemoryAllocateInfo();
	memory_allocation_info.allocationSize  = _memory_requirements.size;
	memory_allocation_info.memoryTypeIndex = renderer->GetGPUMemoryType(_memory_requirements.memoryTypeBits, memory_property_flags);
	ErrorCheck(vkAllocateMemory(renderer->GetDevice(), &memory_allocation_info, nullptr, &_memory), "Unable to allocate GPU memory.");

	// create buffer memory
	if (data != nullptr)
	{
		ErrorCheck(vkMapMemory(renderer->GetDevice(), *&_memory, _offset, buffer_size, 0, &_mapped), "Unable to map GPU memory.");
		memcpy(_mapped, data, _buffer_size);
		vkUnmapMemory(renderer->GetDevice(), _memory);
	}

	// create descriptor set
	_descriptor_info = Structs::DescriptorBufferInfo(_buffer, _buffer_size);

	// bind memory
	ErrorCheck( vkBindBufferMemory(renderer->GetDevice(), *&_buffer, *&_memory, 0), "Unable to bind buffer memory to GPU." );
}

DataBuffer::~DataBuffer()
{
}


void DataBuffer::Update( Renderer * renderer, void * data )
{
	ErrorCheck(vkMapMemory(renderer->GetDevice(), *&_memory, _offset, _buffer_size, 0, &_mapped), "Unable to map GPU memory.");
	memcpy(_mapped, data, _buffer_size);
	vkUnmapMemory(renderer->GetDevice(), _memory);
}


VkDescriptorSet DataBuffer::GetDescriptorSet()
{
	return _descriptor;
}

VkDescriptorBufferInfo * DataBuffer::GetDescriptorInfo()
{
	return &_descriptor_info;
}