#pragma once
#include <Vulkan/vulkan.h>

#include <RenderObjects/Components/RenderObjectData.h>

struct DescriptorData
{
	VkDescriptorSetLayout descriptorLayout;
	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets[2];

	TextureData* texture;
};

struct PipeLineData
{
	VkPipeline pipeLine;
	VkPipelineLayout pipeLineLayout;

	DescriptorData* p_DescriptorData;
};