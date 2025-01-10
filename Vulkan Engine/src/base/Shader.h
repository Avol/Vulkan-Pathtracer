#pragma once



#include "../Platform.h"
#include "../Shared.h"
#include "helpers/Structs.h"

#include <vector>
#include <map>

class Shader
{
	public:
		Shader();
		~Shader();

		static VkPipelineShaderStageCreateInfo LoadShaderStage(const char *fileName, VkDevice device, VkShaderStageFlagBits stage);
		static std::map<const char *, VkShaderModule> _shader_modules;
	//	static std::vector<VkShaderModule> * _shader_mods = { nullptr };
	private:
		
		static VkShaderModule _LoadShaderModule(const char *fileName, VkDevice device, VkShaderStageFlagBits stage);
};

