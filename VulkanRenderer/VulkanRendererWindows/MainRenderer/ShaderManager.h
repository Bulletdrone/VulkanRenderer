#pragma once

#include "Tools/ResourceLoader.h"

#include <Vulkan/vulkan.h>
#include <vector>

class ShaderManager
{
public:
	ShaderManager(const VkDevice& r_VKDevice, const VkExtent2D& r_VKSwapChainExtent, 
		VkDescriptorSetLayout& r_DescriptorSetLayout, VkPipelineLayout& r_VKPipelineLayout, VkPipeline& r_VKPipeline);
	~ShaderManager();

	void CreateGraphicsPipeline(const VkRenderPass& r_RenderPass);
	void CreateDescriptorSetLayout();
	
	VkShaderModule CreateShaderModule(const std::vector<char>& code);

private:
	const VkDevice& rmvk_Device;
	const VkExtent2D& rmvk_SwapChainExtent;

	VkDescriptorSetLayout& rmvk_DescriptorSetLayout;
	VkPipelineLayout& rmvk_PipelineLayout;
	VkPipeline& rmvk_Pipeline;
};