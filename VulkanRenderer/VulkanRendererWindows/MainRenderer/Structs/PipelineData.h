#pragma once
#include <Vulkan/vulkan.h>

#include <RenderObjects/Components/RenderObjectData.h>

struct DescriptorData
{
	uint32_t descID;

	VkDescriptorSetLayout* descriptorLayout;
	VkDescriptorPool descriptorPool;
	VkDescriptorSet descriptorSets[2];

	std::vector<TextureData*> textures;
};

struct PipeLineData
{
	//PipeLineData(const DescriptorData& r_DescData) : r_DescriptorData(r_DescData) {};

	uint32_t pipeID = 0;

	VkPipeline pipeLine = VK_NULL_HANDLE;
	VkPipelineLayout pipeLineLayout = VK_NULL_HANDLE;

	std::vector<DescriptorData*> p_DescriptorData;
};