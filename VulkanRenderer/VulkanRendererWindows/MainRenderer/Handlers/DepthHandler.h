#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Vulkan/vulkan.h>
#include <vector>

struct DepthTest
{
	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;
};

class BufferHandler;
class ImageHandler;

class DepthHandler
{
public:
	DepthHandler(VkDevice& r_Device, VkPhysicalDevice& r_PhysDevice, ImageHandler* a_ImageHandler);
	~DepthHandler();

	void CreateDepthResources(uint32_t a_Width, uint32_t a_Height);


private:
	VkFormat FindSupportedFormat(const std::vector<VkFormat>& r_Candidates, VkImageTiling a_Tiling, VkFormatFeatureFlags a_Features);
	VkFormat FindDepthFormat();
	bool HasStencilComponent(VkFormat a_Format);

	DepthTest m_DepthTest;

	VkPhysicalDevice& rm_PhysDevice;
	VkDevice& rm_Device;

	//Should be reworked.
	ImageHandler* p_ImageHandler;
};

