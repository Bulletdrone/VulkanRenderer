#pragma once
#pragma warning (push, 0)
#include <Vulkan/vulkan.h>
#pragma warning (pop)

namespace VkInit
{
	inline VkDescriptorBufferInfo DescriptorBufferInfo(VkBuffer& r_UniformBuffer, VkDeviceSize a_Offset, VkDeviceSize a_BufferSize)
	{
		VkDescriptorBufferInfo t_BufferInfo{};
		t_BufferInfo.buffer = r_UniformBuffer;
		t_BufferInfo.offset = a_Offset;
		t_BufferInfo.range = a_BufferSize;

		return t_BufferInfo;
	}

	inline VkDescriptorImageInfo DescriptorImageInfo(VkImageLayout a_ImageLayout, VkImageView& r_ImageView, VkSampler& r_Sampler)
	{
		VkDescriptorImageInfo t_ImageInfo{};
		t_ImageInfo.imageLayout = a_ImageLayout;
		t_ImageInfo.imageView = r_ImageView;
		t_ImageInfo.sampler = r_Sampler;

		return t_ImageInfo;
	}


	//Layout Creation.
	inline VkDescriptorSetLayoutBinding DescriptorSetLayoutBinding(VkDescriptorType a_Type, VkShaderStageFlags a_Flags, uint32_t a_Binding)
	{
		VkDescriptorSetLayoutBinding t_LayoutBinding{};
		t_LayoutBinding.binding = a_Binding;
		t_LayoutBinding.descriptorType = a_Type;
		t_LayoutBinding.descriptorCount = 1;
		t_LayoutBinding.stageFlags = a_Flags;
		t_LayoutBinding.pImmutableSamplers = nullptr; // Optional

		return t_LayoutBinding;
	}


	inline VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo(uint32_t a_BindingCount, VkDescriptorSetLayoutBinding* a_LayoutBinding)
	{
		VkDescriptorSetLayoutCreateInfo t_LayoutInfo{};
		t_LayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		t_LayoutInfo.bindingCount = a_BindingCount;
		t_LayoutInfo.pBindings = a_LayoutBinding;

		return t_LayoutInfo;
	}


	//Shader Creation
	inline VkShaderModuleCreateInfo ShaderModuleCreateInfo(const unsigned char* a_ShaderCode, const size_t a_CodeSize)
	{
		VkShaderModuleCreateInfo t_CreateInfo{};
		t_CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		t_CreateInfo.codeSize = a_CodeSize;
		t_CreateInfo.pCode = reinterpret_cast<const uint32_t*>(a_ShaderCode);

		return t_CreateInfo;
	}

	inline VkFramebufferCreateInfo FramebufferCreateInfo()
	{
		VkFramebufferCreateInfo t_FramebufferInfo{};
		t_FramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;

		return t_FramebufferInfo;
	}

	inline VkViewport ViewPort(float a_Width, float a_Height, float a_minDepth, float a_maxDepth)
	{
		VkViewport t_Viewport{};
		t_Viewport.width = a_Width;
		t_Viewport.height = a_Height;
		t_Viewport.minDepth = a_minDepth;
		t_Viewport.maxDepth = a_maxDepth;

		return t_Viewport;
	}

	inline VkRect2D Rect2D(int32_t a_Width, int32_t a_Height, VkExtent2D a_Extend2D)
	{
		VkRect2D t_Rect2D{};
		t_Rect2D.offset.x = a_Width;
		t_Rect2D.offset.y = a_Height;
		t_Rect2D.extent = a_Extend2D;

		return t_Rect2D;
	}

	inline VkRect2D Rect2D(int32_t a_Width, int32_t a_Height, int32_t a_OffsetX, int32_t a_OffsetY)
	{
		VkRect2D t_Rect2D{};
		t_Rect2D.offset.x = a_Width;
		t_Rect2D.offset.y = a_Height;
		t_Rect2D.extent.width = a_OffsetX;
		t_Rect2D.extent.height = a_OffsetY;

		return t_Rect2D;
	}

#pragma region Pipeline Creation

	inline VkPipelineVertexInputStateCreateInfo PipeLineVertexInputStateCreateInfo()
	{
		VkPipelineVertexInputStateCreateInfo t_VertexInputInfo{};
		t_VertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		return t_VertexInputInfo;
	}

	inline VkPipelineVertexInputStateCreateInfo PipeLineVertexInputStateCreateInfo(
		uint32_t a_BindingCount, VkVertexInputBindingDescription* a_Bindings,
		uint32_t a_AttributeCount, VkVertexInputAttributeDescription *a_Attributes)
	{
		VkPipelineVertexInputStateCreateInfo t_VertexInputInfo{};
		t_VertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		t_VertexInputInfo.vertexBindingDescriptionCount = a_BindingCount;
		t_VertexInputInfo.pVertexBindingDescriptions = a_Bindings;
		t_VertexInputInfo.vertexAttributeDescriptionCount = a_AttributeCount;
		t_VertexInputInfo.pVertexAttributeDescriptions = a_Attributes;

		return t_VertexInputInfo;
	}


#pragma endregion Pipeline Creation
}