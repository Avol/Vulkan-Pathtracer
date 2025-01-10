#pragma once

#include "Platform.h"
#include "Renderer.h"
#include "Shared.h"

class Renderer;
class Texture
{
	private:
		VkImage							_image;
		VkImageView						_image_view;
		VkDeviceMemory					_memory;
		VkSampler						_sampler;
		VkDescriptorImageInfo           _descriptor;

		void							_CreateImage(Renderer * renderer, uint32_t width, uint32_t height, VkFormat format);
		void							_CreateImageView(Renderer * renderer, VkFormat format, VkImageAspectFlagBits aspectMask);
		void							_CreateImageMemory(Renderer * renderer);
		void							_CreateSampler(Renderer * renderer);
		void							_CreateImageDescriptor();
		void							_CopyTextureData(Renderer * renderer, std::vector<char> texture_data, uint32_t width, uint32_t height);

		//static std::vector<char>		_GetImageContents(std::string file_name);

	public:
		Texture( Renderer * renderer, uint32_t width, uint32_t height, VkFormat format, std::vector<char> texture_data = std::vector<char>(), VkImageAspectFlagBits aspectMask = VK_IMAGE_ASPECT_COLOR_BIT);
		~Texture();

		static Texture  	*			Load( Renderer * renderer, std::string file_name );

		void							Clear( VkClearColorValue color );

		VkImage							GetImage();
		VkImageView						GetImageView();
		VkDescriptorImageInfo           GetDescriptor();
};

