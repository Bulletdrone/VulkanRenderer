#pragma once
#include "VulkanDevice.h"

class VulkanSwapChain
{
private:
	VulkanDevice& rm_VulkanDevice;

public:
	VulkanSwapChain(VulkanDevice& r_VulkanDevice, size_t a_FrameAmount);

	//The SwapChain for buffering images and more.
	VkSwapchainKHR SwapChain;

	VkFormat SwapChainImageFormat;
	VkExtent2D SwapChainExtent;

	//Vectors
	std::vector<VkImage> SwapChainImages;
	std::vector<VkImageView> SwapChainImageViews;

	std::vector<VkFramebuffer> SwapChainFrameBuffers;
};

