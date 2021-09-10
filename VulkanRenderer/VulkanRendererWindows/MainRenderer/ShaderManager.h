#pragma once

#include "Tools/ResourceLoader.h"
#include "Structs/PipelineData.h"
#include "Tools/ObjectPool.h"

#include "VulkanDevice.h"

struct DescriptorLayout
{
	uint32_t ID;

	VkDescriptorSetLayout descriptorSetLayout;
	uint32_t bufferCount;
	uint32_t imageCount;

	bool SameLayout(uint32_t a_BufferCount, uint32_t a_ImageCount)
	{
		if (bufferCount == a_BufferCount && imageCount == a_ImageCount)
			return true;

		return false;
	}
};

class ShaderManager
{
public:
	ShaderManager(VulkanDevice& r_VulkanDevice, const VkExtent2D& r_VKSwapChainExtent);
	~ShaderManager();

	/*	Create a DescriptorData inside the ShaderManager, returns the ID of the location.
	@param a_TextureDatas, The Textures that the Descriptor needs, can be 0.
	@param a_BufferCount, the amount of uniform buffers, can be 0.
	@param p_DescriptorPointer, an nullptr of a VkDescriptorSetLayout, returns a new or already existing layout.
	@return the uint32_t ID of the DescriptorData location. */
	uint32_t CreateDescriptorData(std::vector<TextureData>& a_TextureData, uint32_t a_BufferCount, VkDescriptorSetLayout* p_DescriptorPointer);

	uint32_t CreatePipelineData(const VkRenderPass& r_RenderPass, std::vector<uint32_t>& a_DescriptorIDs);
	void RecreatePipelines(const VkRenderPass& r_RenderPass);

	void CreateDescriptorPool(const size_t a_FrameAmount, uint32_t a_DescID);
	void CreateDescriptorSet(const size_t a_FrameAmount, uint32_t a_DescID, std::vector<VkDescriptorBufferInfo>* a_Buffers, std::vector<VkDescriptorImageInfo>* a_Images);
	void RecreateDescriptors(const size_t a_FrameAmount, std::vector<VkBuffer>& r_UniformBuffers, VkDeviceSize a_BufferSize);

	VkShaderModule CreateShaderModule(const std::vector<char>& code);

public:
	ObjectPool<PipeLineData> PipelinePool{ ObjectPool<PipeLineData>(10, 5) };
	ObjectPool<DescriptorData> DescriptorPool{ ObjectPool<DescriptorData>(10, 5) };

	std::vector<DescriptorLayout> DescriptorLayouts;

private:
	void CreateGraphicsPipeline(const VkRenderPass& r_RenderPass, PipeLineData& r_PipeLineData);
	VkDescriptorSetLayout* CreateDescriptorLayout(const uint32_t a_BufferCount, const uint32_t a_ImageCount);

	VulkanDevice& rm_VulkanDevice;
	const VkExtent2D& rmvk_SwapChainExtent;
};