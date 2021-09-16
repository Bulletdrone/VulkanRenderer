#include "Material.h"

Material::Material(uint32_t a_PipelineID, uint32_t a_SecondDescriptorSetID)
{
	m_PipelineID = a_PipelineID;
	m_SecondDescriptorSetID = a_SecondDescriptorSetID;

	m_AmbientColor = glm::vec4(0);
}

Material::Material(uint32_t a_PipelineID, uint32_t a_SecondDescriptorSetID, glm::vec4& a_AmbientColor)
{
	m_PipelineID = a_PipelineID;
	m_SecondDescriptorSetID = a_SecondDescriptorSetID;

	m_AmbientColor = a_AmbientColor;
}

Material::~Material()
{}