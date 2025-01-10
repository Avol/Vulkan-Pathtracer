#pragma once

#include <iostream>
#include <assert.h>
#include <vulkan/vulkan.h>

void ErrorCheck(VkResult result, std::string error_message = "", std::string success_message = "");



