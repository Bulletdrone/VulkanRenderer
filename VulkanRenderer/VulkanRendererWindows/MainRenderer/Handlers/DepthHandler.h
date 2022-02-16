#pragma once

#pragma warning (push, 0)
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#pragma warning (pop)

#include "../VulkanDevice.h"

struct DepthTest
{
	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;
};

class ImageHandler;

class DepthHandler
{
public:
	DepthHandler(VulkanDevice& r_VulkanDevice, ImageHandler* a_ImageHandler);
	~DepthHandler();

	void CleanupDepthTest();

	void CreateDepthResources(uint32_t a_Width, uint32_t a_Height);

	VkFormat FindDepthFormat();

	DepthTest& GetDepthTest();
private:
	VkFormat FindSupportedFormat(const std::vector<VkFormat>& r_Candidates, VkImageTiling a_Tiling, VkFormatFeatureFlags a_Features);

	DepthTest m_DepthTest;

	VulkanDevice& rm_VulkanDevice;

	//Should be reworked.
	ImageHandler* p_ImageHandler;
};

