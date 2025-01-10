#include "PathTracer.h"
#include <random>

// cons & dest
PathTracer::PathTracer(Renderer * renderer, uint32_t width, uint32_t height)
{
	_renderer									= renderer;
	
	_camera										= new Camera( renderer->GetWindow(), glm::vec2( width, height ) );

	std::cout << "--------------------------------------------- Creating scene data ------------------------------------------" << std::endl;

	_uniform_general.time							    = 0.0f;
	_uniform_general.resolution					        = glm::vec2(width, height);
	_uniform_general.inverse_projection_view            = _camera->GetInverseProjectionView();
	_uniform_general_buffer								= new DataBuffer(renderer, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, &_uniform_general, sizeof(General));

	_uniform_light.type                                 = 0;
	_uniform_light.position                             = glm::vec4(0.0f, 1.0f, 1.0f, 0);
	_uniform_light.direction                            = glm::vec4(0.0f);
	_uniform_light.color                                = glm::vec4(0.5f);
	_uniform_light.radius                               = 4.0f;
	_uniform_light.constantAttenuation                  = 0.0f;
	_uniform_light.linearAttenuation                    = 0.2f;
	_uniform_light.quadraticAttenuation                 = 3.0f;
	_uniform_light_buffer                               = new DataBuffer(renderer, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, &_uniform_light, sizeof(Light));

	// green floor
	Plane plane0;
	plane0.position          = glm::vec4(0, -0.5, 0.0f, 1.0f);
	plane0.normal            = glm::vec4(0, 1.0f, 0, 0.0f);
	plane0.albedo            = glm::vec4(0.2f, 1.0f, 0.2f, 1.0f);
	plane0.specular          = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	plane0.redf              = glm::vec4(0.4f, 0.0f, 0.0, 0.025f);
	
	// blue ceiling
	Plane plane1;
	plane1.position          = glm::vec4(0, 1.5f, 0.0f, 1.0f);
	plane1.normal            = glm::vec4(0, -1.0f, 0, 0.0f);
	plane1.albedo            = glm::vec4(0.2f, 0.2f, 1.0f, 1.0f);
	plane1.specular          = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	plane1.redf              = glm::vec4(0.95f, 0.0f, 0.0, 0.025f);

	// red right
	Plane plane4;
	plane4.position          = glm::vec4(2.0f, 0.0f, 0, 1.0f);
	plane4.normal            = glm::vec4(-1.0f, 0.0f, 0.0, 0.0f);
	plane4.albedo            = glm::vec4(1.0f, 0.2f, 0.2f, 1.0f);
	plane4.specular          = glm::vec4(1.0f, 0.1f, 0.1f, 0.0f);
	plane4.redf              = glm::vec4(0.95f, 0.0f, 0.0f, 0.025f);
	
	// mirror
	Plane plane2;
	plane2.position          = glm::vec4(0, 0.0f, -1.0f, 1.0f);
	plane2.normal            = glm::vec4(0, 0.0f, 1.0, 0.0f);
	plane2.albedo            = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	plane2.specular          = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	plane2.redf              = glm::vec4(0.05f, 0.0f, 0.0f, 0.025f);

	//translucent
	Plane plane3;
	plane3.position          = glm::vec4(-1.5f, 0.0f, 0, 1.0f);
	plane3.normal            = glm::vec4(1.0f, 0.0f, 0.0, 0.0f);
	plane3.albedo            = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	plane3.specular          = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	plane3.redf              = glm::vec4(0.3f, 0.0f, 0.0f, 0.025f);

	//bck
	Plane plane5;
	plane5.position          = glm::vec4(0, 0.0f, 4.0f, 1.0f);
	plane5.normal            = glm::vec4(0.0f, 0.0f, -1.0, 0.0f);
	plane5.albedo            = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	plane5.specular          = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	plane5.redf              = glm::vec4(0.99f, 0.0f, 0.0f, 0.025f);

	///////// SPHERES ////////////////

	// reflective
	Sphere sphere_0;
	sphere_0.position         = glm::vec4(0.0f, 0.25f, -0.5f, 0.4f); // lst component scle
	sphere_0.albedo           = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	sphere_0.specular         = glm::vec4(15.0f, 15.0f, 15.0f, 1.0f);
	sphere_0.redf             = glm::vec4(0.0f, 0.0f, 0.0, 0.025f);

	// trnslucent
	Sphere sphere_1;
	sphere_1.position         = glm::vec4(1.1f, 0.2f, 0.3f, 0.4f);
	sphere_1.albedo           = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	sphere_1.specular         = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	sphere_1.redf             = glm::vec4(1.0f, 0.0f, 0.0f, 0.025f);

	// trnslucent
	Sphere sphere_2;
	sphere_2.position         = glm::vec4(0.5f, 0.0f, 1.0f, 0.4f);
	sphere_2.albedo           = glm::vec4(0.9f, 0.0f, 0.0f, 0.0f);
	sphere_2.specular         = glm::vec4(3.0f, 3.0f, 3.0f, 0.0f);
	sphere_2.redf             = glm::vec4(0.2f, 0.0f, 0.0f, 0.025f);

	// white
	Sphere sphere_3;
	sphere_3.position = glm::vec4(0.5f, 0.0f, 3.3f, 0.4f);
	sphere_3.albedo = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	sphere_3.specular = glm::vec4(3.0f, 3.0f, 3.0f, 0.0f);
	sphere_3.redf = glm::vec4(0.2f, 0.0f, 0.0f, 0.025f);


	_uniform_planes.planes[0] = plane0;
	_uniform_planes.planes[1] = plane1;
	_uniform_planes.planes[2] = plane2;
	_uniform_planes.planes[3] = plane3;
	_uniform_planes.planes[4] = plane4;
	_uniform_planes.planes[5] = plane5;

	_uniform_spheres.spheres[0] = sphere_0;
	_uniform_spheres.spheres[1] = sphere_1;
	_uniform_spheres.spheres[2] = sphere_2;
	_uniform_spheres.spheres[3] = sphere_3;

	_uniform_planes_buffer                              = new DataBuffer(renderer, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, &_uniform_planes, sizeof(Planes));
	_uniform_spheres_buffer                             = new DataBuffer(renderer, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, &_uniform_spheres, sizeof(Spheres));


	std::cout << "-------------------------------------- Creating a compute shader pipeline -----------------------------------" << std::endl;

	_CreateDescriptorSetLayouts();
	_CreatePipelineLayout();
	_CreateDescriptorPool();

	_descriptor_sets.resize(2);
	_AllocateDescriptorSets(); 

	_CreatePipelineCache();
	_CreatePipeline();

	std::cout << "-------------------------------------- Creating compute buffers, pool, fence -----------------------------------" << std::endl;

	_CreateCommandPoolAndBuffers();
	_RecordCommandBuffers();
	_CreateFence();
}

PathTracer::~PathTracer()
{
}


// descriptors
void PathTracer::_CreateDescriptorSetLayouts()
{
	std::vector<VkDescriptorSetLayoutBinding> set_layout_bindings = 
	{ 
		Structs::DescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT, 0),
		Structs::DescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT, 1),
		Structs::DescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 2),
		Structs::DescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 3),
		Structs::DescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 4),
		Structs::DescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 5)
	};

	VkDescriptorSetLayoutCreateInfo create_info = Structs::DescriptorSetLayoutCreateInfo(set_layout_bindings);
	ErrorCheck( vkCreateDescriptorSetLayout( _renderer->GetDevice(), &create_info, nullptr, &_descriptor_set_layout),
											"Unable to crete descriptor set layout.", "Descriptor set layout created." );
}

void PathTracer::_CreateDescriptorPool()
{
	std::vector<VkDescriptorPoolSize> poolSizes =
	{
		Structs::DescriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2),			// required for uniforms dfq?
		Structs::DescriptorPoolSize(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 2),					// uniforms
		Structs::DescriptorPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 2),					// Compute pipelines uses a storage image for image reads and writes
		Structs::DescriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 8),					// uniforms
	};

	VkDescriptorPoolCreateInfo descriptorPoolInfo = Structs::DescriptorPoolCreateInfo(poolSizes);
	ErrorCheck( vkCreateDescriptorPool( _renderer->GetDevice(), &descriptorPoolInfo, nullptr, &_descriptor_pool),
										"Unable to create descriptor pool.", "Descriptor pool created." );
}

void PathTracer::_AllocateDescriptorSets()
{
	VkDescriptorSetAllocateInfo allocate_info = Structs::DescriptorSetAllocateInfo(_descriptor_pool, _descriptor_set_layout);

	// alocate descriptor sets for 2 swapchain images.
	for (int i = 0; i < 2; i++)
	{
		//  sampled image
		ErrorCheck(vkAllocateDescriptorSets(_renderer->GetDevice(), &allocate_info, &_descriptor_sets[i]),
			"Unable to allocate descriptor set.", "Descriptor set allocated image.");


		std::vector<VkWriteDescriptorSet> computeWriteDescriptorSets =
		{
			Structs::WriteDescriptorSet(_descriptor_sets[i], VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 0, &_renderer->GetWindow()->GetPresentation()->GetPresentationImageDescriptor(i == 0 ? 1 : 0)),             // Binding 0 : Sampled image (read)
			Structs::WriteDescriptorSet(_descriptor_sets[i], VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, &_renderer->GetWindow()->GetPresentation()->GetPresentationImageDescriptor(i)),			// Binding 1 : Sampled image (write)
			Structs::WriteDescriptorSet(_descriptor_sets[i], VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2, _uniform_general_buffer->GetDescriptorInfo()),			
			Structs::WriteDescriptorSet(_descriptor_sets[i], VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3, _uniform_light_buffer->GetDescriptorInfo()),	
			Structs::WriteDescriptorSet(_descriptor_sets[i], VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 4, _uniform_planes_buffer->GetDescriptorInfo()),
			Structs::WriteDescriptorSet(_descriptor_sets[i], VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 5, _uniform_spheres_buffer->GetDescriptorInfo())
		};

		vkUpdateDescriptorSets( _renderer->GetDevice(), (uint32_t)computeWriteDescriptorSets.size(), computeWriteDescriptorSets.data(), 0, NULL );
	}
}


// pipeline
void PathTracer::_CreatePipelineLayout()
{
	VkPipelineLayoutCreateInfo create_info = Structs::PipelineLayoutCreateInfo(_descriptor_set_layout);
	ErrorCheck( vkCreatePipelineLayout(_renderer->GetDevice(), &create_info, nullptr, &_pipeline_layout),
				"Unable to create compute pipeline layout.", "Compute pipeline layout has been created." );
}

void PathTracer::_CreatePipelineCache()
{
	VkPipelineCacheCreateInfo create_info = Structs::PipelineCacheCreateInfo();
	ErrorCheck( vkCreatePipelineCache(_renderer->GetDevice(), &create_info, nullptr, &_pipeline_cache),
				"Unable to create pipeline cache.", "Pipeline cache has been created." );
}

void PathTracer::_CreatePipeline()
{
	VkComputePipelineCreateInfo create_info = Structs::ComputePipelineCreateInfo(_pipeline_layout);

	std::vector<std::string> shaderNames = { "pathtracer" };	// One pipeline for shader
	for (auto& shaderName : shaderNames)
	{
		std::string fileName	= "shaders/" + shaderName + ".comp.spv";
		create_info.stage		= Shader::LoadShaderStage(fileName.c_str() , _renderer->GetDevice(), VK_SHADER_STAGE_COMPUTE_BIT);

		VkPipeline pipeline;
		ErrorCheck( vkCreateComputePipelines( _renderer->GetDevice(), _pipeline_cache, 1, &create_info, nullptr, &pipeline ), "Unable to create compute pipeline.", "Compute pipeline created");

		_pipelines.push_back(pipeline);
	}
}


// command buffers
void PathTracer::_CreateCommandPoolAndBuffers()
{
	_command_buffers.resize(2);

	VkCommandPoolCreateInfo create_info = Structs::CommandPoolCreateInfo(_renderer->GetComputeFamilyIndex() );
	ErrorCheck(vkCreateCommandPool(_renderer->GetDevice(), &create_info, nullptr, &_command_pool),
		"Unable to create a compute command pool.", "Compute queue command pool created.");

	VkCommandBufferAllocateInfo allocate_info = Structs::CommandBufferAllocateInfo( _command_pool, 2 );
	ErrorCheck(vkAllocateCommandBuffers(_renderer->GetDevice(), &allocate_info, &_command_buffers[0]),
		"Unable to allocate compute command buffers.", "Compute Command buffers have been allocated.");
}

void PathTracer::_RecordCommandBuffers()
{
	vkQueueWaitIdle( _renderer->GetComputeQueue() );
	
	VkCommandBufferBeginInfo cmd_buffer_begin_info = Structs::CommandBufferBeginInfo();

	VkImageSubresourceRange image_subresource_range = Structs::ImageSubresourceRange( VK_IMAGE_ASPECT_COLOR_BIT );

	for (uint32_t i = 0; i < (uint32_t)_renderer->GetWindow()->GetPresentation()->GetSwapchainImages().size(); ++i) {

		VkImageMemoryBarrier barrier_from_present_to_clear = {
			VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,     // VkStructureType                        sType
			nullptr,                                    // const void                            *pNext
			VK_ACCESS_MEMORY_READ_BIT,                  // VkAccessFlags                          srcAccessMask
			VK_ACCESS_TRANSFER_WRITE_BIT,               // VkAccessFlags                          dstAccessMask
			VK_IMAGE_LAYOUT_UNDEFINED,                    // VkImageLayout                          oldLayout
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,       // VkImageLayout                          newLayout
			VK_QUEUE_FAMILY_IGNORED,                    // uint32_t                               srcQueueFamilyIndex
			VK_QUEUE_FAMILY_IGNORED,                    // uint32_t                               dstQueueFamilyIndex
			_renderer->GetWindow()->GetPresentation()->GetSwapchainImages()[i],                       // VkImage                                image
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
			_renderer->GetWindow()->GetPresentation()->GetSwapchainImages()[i],                       // VkImage                                image
			image_subresource_range                     // VkImageSubresourceRange                subresourceRange
		};
	

		ErrorCheck(vkBeginCommandBuffer(_command_buffers[i], &cmd_buffer_begin_info),
			"Unable to create command buffer begin info.", "Compute command buffers created.");


		vkCmdPipelineBarrier(_command_buffers[i], VK_ACCESS_MEMORY_WRITE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier_from_present_to_clear);

		vkCmdBindPipeline(_command_buffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, _pipelines[_pipeline_index]);
		vkCmdBindDescriptorSets(_command_buffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, _pipeline_layout, 0, 1, &_descriptor_sets[i], 0, 0);

		vkCmdPipelineBarrier(_command_buffers[i], VK_ACCESS_MEMORY_READ_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier_from_clear_to_present);

		vkCmdDispatch(_command_buffers[i], 800 / 16, 610 / 16, 1);

		vkEndCommandBuffer(_command_buffers[i]);
	}
}

void PathTracer::_CreateFence()
{
	VkFenceCreateInfo fence_create_info = Structs::FenceCreateInfo();
	ErrorCheck( vkCreateFence(_renderer->GetDevice(), &fence_create_info, nullptr, &_fence),
		"Unable to create compute fence.", "Compute fence successfully created" );
}

void PathTracer::Dispatch()
{
	// update camera
	bool updated = _camera->Update();

	// do stuff with uniforms
	_uniform_general.inverse_projection_view = _camera->GetInverseProjectionView();
	updated ? _uniform_general.frame = 0 : _uniform_general.frame += 1;
	_uniform_general.time += 0.01f;
	_uniform_general_buffer->Update(_renderer, &_uniform_general);
	//_uniform_light_buffer->Update(_renderer, &_uniform_light);


	// prepare frame
	uint32_t image_index;
	image_index = _renderer->GetWindow()->GetPresentation()->PrepareFrame();

	// prepare fences
	vkWaitForFences(_renderer->GetDevice(), 1, &_fence, VK_TRUE, UINT64_MAX);
	vkResetFences(_renderer->GetDevice(), 1, &_fence);

	// submit queue
	VkSubmitInfo submit_info = Structs::SubmitInfo( _command_buffers[image_index],
													_renderer->GetWindow()->GetPresentation()->GetSemaphoreImageAvailable(),
													_renderer->GetWindow()->GetPresentation()->GetSemaphoreRenderingFinished(),
													{ VK_PIPELINE_STAGE_TRANSFER_BIT });

	ErrorCheck( vkQueueSubmit(_renderer->GetComputeQueue(), 1, &submit_info, _fence), "Unable to submit compute queue" );

	// render frame to screen
	_renderer->GetWindow()->GetPresentation()->RenderFrame(image_index);
}