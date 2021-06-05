#pragma once

#include "MeshData.h"

struct TextureData 
{
	void DeleteBuffers(VkDevice& r_Device)
	{
		vkDestroySampler(r_Device, textureSampler, nullptr);
		vkDestroyImageView(r_Device, textureImageView, nullptr);

		vkDestroyImage(r_Device, textureImage, nullptr);
		vkFreeMemory(r_Device, textureImageMemory, nullptr);
	}

	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	VkImageView textureImageView;
	VkSampler textureSampler;
};

//Pre-build renderdata that is created on startup. 
//This will include loaded files and basic shapes.
//You can assign some parts yourself like a texture or color depending what you want.
//RenderObjectData handles it's own deletion, so all the parts here shouldn't be deleted.
struct RenderObjectData
{
	RenderObjectData(MeshData* a_MeshData) 
		: m_MeshData(a_MeshData) {}

	RenderObjectData(MeshData* a_MeshData, TextureData* a_TexData)
		: m_MeshData(a_MeshData), p_TexData(a_TexData) {}

	RenderObjectData(MeshData* p_MeshData, glm::vec4 a_Color)
		: m_MeshData(p_MeshData), m_Color(a_Color) {}

	~RenderObjectData() { delete m_MeshData; };

	MeshData* m_MeshData;
	TextureData* p_TexData = nullptr;

	glm::vec4 m_Color = glm::vec4(1);
};