#include "Shader.h"

std::map<const char *, VkShaderModule> Shader::_shader_modules;


Shader::Shader()
{
}

Shader::~Shader()
{
}


// shaders
VkShaderModule Shader::_LoadShaderModule(const char *fileName, VkDevice device, VkShaderStageFlagBits stage)
{
	size_t size;
	FILE *fp;
	int err = fopen_s(&fp, fileName, "rb");

	if (err != 0)		std::cout << "Unable to open a file: " << fileName << std::endl;

	assert(&fp);

	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);

	if (size == 0)		std::cout << "The file is empty: " << fileName << std::endl;

	fseek(fp, 0L, SEEK_SET);

	//shaderCode = malloc(size);
	char *shaderCode = new char[size];
	size_t retval = fread(shaderCode, size, 1, fp);

	assert(retval == 1);
	assert(size > 0);

	fclose(fp);

	VkShaderModule shaderModule;
	VkShaderModuleCreateInfo moduleCreateInfo;
	moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	moduleCreateInfo.pNext = NULL;
	moduleCreateInfo.codeSize = size;
	moduleCreateInfo.pCode = (uint32_t*)shaderCode;
	moduleCreateInfo.flags = 0;

	ErrorCheck(vkCreateShaderModule(device, &moduleCreateInfo, NULL, &shaderModule), "Unable to create a shader module.", "Shader module has been created from file: " + (std::string)fileName);

	delete[] shaderCode;

	return shaderModule;
}

VkPipelineShaderStageCreateInfo Shader::LoadShaderStage(const char *fileName, VkDevice device, VkShaderStageFlagBits stage)
{
	// load a new shader module or reuse
	VkShaderModule shader_module;

	// make sure we haven't loaded the shader module yet.
	if (!_shader_modules.count(fileName))			shader_module = _LoadShaderModule(fileName, device, stage);
	else											shader_module = _shader_modules.at(fileName);

	// store for later usage
	_shader_modules.insert( std::make_pair(fileName, shader_module) );

	// construct pipeline shater stage info.
	return Structs::PipelineShaderStageCreateInfo("main", stage, shader_module);
}
