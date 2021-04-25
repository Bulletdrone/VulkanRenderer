#pragma once

#include "Tools/ResourceLoader.h"

#include <Vulkan/vulkan.h>
#include <vector>

class ShaderManager
{
public:
	ShaderManager(const VkDevice& r_VKDevice, const VkExtent2D& r_VKSwapChainExtent, 
		VkPipelineLayout& r_VKPipelineLayout, VkPipeline& r_VKPipeline);
	~ShaderManager();


	void CreateGraphicsPipeline(const VkRenderPass& r_RenderPass);
	VkShaderModule CreateShaderModule(const std::vector<char>& code);
	void DestroyShaderModules();

	void GetShaderModules(VkShaderModule& r_VertShad, VkShaderModule& r_FragShad);

private:
	const VkDevice& rm_VKDevice;
	const VkExtent2D& rm_VKSwapChainExtent;

	VkPipelineLayout& rm_VKPipelineLayout;
	VkPipeline& rm_VKPipeline;

	//VkShaderModule sh_Unlit[2];

};