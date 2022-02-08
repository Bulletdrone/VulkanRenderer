#pragma once
#pragma warning (push, 0)
#include <Vulkan/vulkan.h>
#pragma warning (pop)

struct Shader
{
	VkShaderModule shaderModule = VK_NULL_HANDLE;
};