#pragma once
#pragma warning (push, 0)
#include <Vulkan/vulkan.h>
#include <glm/glm.hpp>
#pragma warning (pop)

constexpr uint32_t MATERIALBINDING = 1;

class Material
{
public:
	uint32_t pipelineID;
	VkDescriptorSet secondDescriptorSet = VK_NULL_HANDLE;
	glm::vec4 ambientColor = glm::vec4(0, 0, 0, 1); //w is transparency.
};