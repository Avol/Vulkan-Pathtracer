#pragma once


#include <vector>
#include <glm\glm.hpp>

#include "Platform.h"
#include "Shared.h"
#include "Texture.h"

#include "base\Shader.h"
#include "base\DataBuffer.h"
#include "base\helpers\Structs.h"
#include "../Camera.h"

class PathTracer
{
public:
	struct General
	{
		glm::mat4x4   inverse_projection_view;
		glm::vec2     resolution;
		int           frame;
		float         time;
	};

	struct Light
	{
		glm::vec4      position;
		glm::vec4      color;
		glm::vec4      direction;
		float          radius;
		float          constantAttenuation;
		float          linearAttenuation;
		float          quadraticAttenuation;
		int            type;
	};

	struct Plane
	{
		glm::vec4 normal;
		glm::vec4 position;
		glm::vec4 albedo;
		glm::vec4 specular;
		glm::vec4 redf;      // reflection, emission, decay, fresnel
	};

	struct Sphere
	{
		glm::vec4 position;
		glm::vec4 albedo;
		glm::vec4 specular;
		glm::vec4 redf;      // reflection, emission, decay, fresnel
	};

	struct Planes
	{
		Plane            planes[6];
	};

	struct Spheres
	{
		Sphere           spheres[4];
	};



	private:
		General         					_uniform_general = {};
		Light                               _uniform_light = {};
		Planes                              _uniform_planes = {};
		Spheres                             _uniform_spheres = {};

		DataBuffer				*			_uniform_general_buffer;
		DataBuffer              *           _uniform_light_buffer;
		DataBuffer              *           _uniform_planes_buffer;
		DataBuffer              *           _uniform_spheres_buffer;


		Renderer				*			_renderer								= nullptr;
		Camera					*			_camera									= nullptr;

		VkCommandPool			            _command_pool							= VK_NULL_HANDLE;
		std::vector<VkCommandBuffer>		_command_buffers;				

		VkFence								_fence									= VK_NULL_HANDLE;
		VkDescriptorSetLayout				_descriptor_set_layout					= VK_NULL_HANDLE;
		std::vector<VkDescriptorSet>		_descriptor_sets;				
		VkPipelineLayout					_pipeline_layout						= VK_NULL_HANDLE;
		std::vector<VkPipeline>				_pipelines;								
		uint32_t							_pipeline_index							= 0;
		VkDescriptorPool					_descriptor_pool						= VK_NULL_HANDLE;

		std::vector<VkShaderModule>			_shader_modules;
		VkPipelineCache						_pipeline_cache							= VK_NULL_HANDLE;

	private:

		void _CreateDescriptorPool();
		void _CreateDescriptorSetLayouts();
		void _AllocateDescriptorSets();

		void _CreatePipelineLayout();
		void _CreatePipelineCache();
		void _CreatePipeline();

		void _CreateCommandPoolAndBuffers();
		void _RecordCommandBuffers();
		void _CreateFence();

	public:
		PathTracer(Renderer * renderer, uint32_t width, uint32_t height);
		~PathTracer();

		void Dispatch();
};

