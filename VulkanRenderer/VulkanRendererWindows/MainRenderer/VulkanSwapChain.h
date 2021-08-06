#pragma once
#include "VulkanDevice.h"
#include "Handlers/ImageHandler.h"

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class VulkanSwapChain
{
private:
	VulkanDevice* p_VulkanDevice;

public:
	VulkanSwapChain();

	void ConnectVulkanDevice(VulkanDevice* a_VulkanDevice);
	
	void CreateSwapChain(Window& r_Window, const size_t a_FrameAmount);
	void CreateImageViews(ImageHandler* p_ImageHandler);

	//The SwapChain for buffering images and more.
	VkSwapchainKHR SwapChain = VK_NULL_HANDLE;

	VkFormat SwapChainImageFormat;
	VkExtent2D SwapChainExtent;

	//Vectors
	std::vector<VkImage> SwapChainImages;
	std::vector<VkImageView> SwapChainImageViews;

	std::vector<VkFramebuffer> SwapChainFrameBuffers;

private:
	//SwapChain Setup
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& a_AvailableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& a_AvailablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& a_Capabilities, Window& r_Window);
};

//Special function for SwapChain that is accessible everywhere.
inline SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice& r_PhysicalDevice, Window& r_Window) {
	SwapChainSupportDetails t_Details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(r_PhysicalDevice, r_Window.GetSurface(), &t_Details.capabilities);

	//Get Format Data
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(r_PhysicalDevice, r_Window.GetSurface(), &formatCount, nullptr);

	if (formatCount != 0)
	{
		t_Details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(r_PhysicalDevice, r_Window.GetSurface(), &formatCount, t_Details.formats.data());
	}

	//Get Present Data
	uint32_t t_PresentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(r_PhysicalDevice, r_Window.GetSurface(), &t_PresentModeCount, nullptr);

	if (t_PresentModeCount != 0)
	{
		t_Details.presentModes.resize(t_PresentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(r_PhysicalDevice, r_Window.GetSurface(), &t_PresentModeCount, t_Details.presentModes.data());
	}

	return t_Details;
}