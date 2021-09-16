#pragma once
#include <Vulkan/vulkan.h>
#include <glm/glm.hpp>

class Material
{
public:
	Material(uint32_t a_PipelineID, uint32_t a_SecondDescriptorSetID);
	Material(uint32_t a_PipelineID, uint32_t a_SecondDescriptorSetID, glm::vec4& a_AmbientColor);
	~Material();



	uint32_t GetPipelineID() { return m_PipelineID; }
	uint32_t GetDescriptorSetID() { return m_SecondDescriptorSetID; }

private:
	uint32_t m_PipelineID;
	uint32_t m_SecondDescriptorSetID;

	glm::vec4 m_AmbientColor; //w is transparency.
};