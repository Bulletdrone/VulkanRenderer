#pragma once
#include <Vulkan/vulkan.h>
#include <glm/glm.hpp>

class Material
{
public:
	Material();
	~Material();

	//Returns if it was successful.
	bool RecreateDescriptorSet();

	VkDescriptorSet& GetDescriptorSet() { return m_SecondDescriptorSet; }

public:
	uint32_t PipelineID;

private:

	VkDescriptorSet m_SecondDescriptorSet{};

	glm::vec4 m_AmbientColor = glm::vec4(0, 0, 0, 1); //w is transparency.
};