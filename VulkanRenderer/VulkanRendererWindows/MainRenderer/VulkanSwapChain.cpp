#include "VulkanSwapChain.h"

#include <stdexcept>
#include <algorithm>

VulkanSwapChain::VulkanSwapChain()
{}

void VulkanSwapChain::ConnectVulkanDevice(VulkanDevice* a_VulkanDevice)
{
	p_VulkanDevice = a_VulkanDevice;
}


void VulkanSwapChain::CreateSwapChain(Window& r_Window, const uint32_t a_FrameAmount)
{
	VkSwapchainKHR t_OldSwapChain = SwapChain;

	SwapChainSupportDetails t_SwapChainSupport = QuerySwapChainSupport(p_VulkanDevice->m_PhysicalDevice, r_Window);

	//Getting the needed values from the PhysicalDevice.
	VkSurfaceFormatKHR t_SurfaceFormat = ChooseSwapSurfaceFormat(t_SwapChainSupport.formats);
	VkPresentModeKHR t_PresentMode = ChooseSwapPresentMode(t_SwapChainSupport.presentModes);
	VkExtent2D t_Extent = ChooseSwapExtent(t_SwapChainSupport.capabilities, r_Window);

	//Setting up the SwapChain.
	uint32_t t_ImageCount = t_SwapChainSupport.capabilities.minImageCount + 1;
	uint32_t t_ActualImageCount = a_FrameAmount;

	if (t_SwapChainSupport.capabilities.maxImageCount > 0 && t_ImageCount > t_SwapChainSupport.capabilities.maxImageCount)
	{
		t_ImageCount = t_SwapChainSupport.capabilities.maxImageCount;

		//Check if the swapchai can handle at least 2 images.
		if (t_SwapChainSupport.capabilities.maxImageCount < 2)
		{
			throw std::runtime_error("maxImageCount is under 2!");
		}

	}

	VkSwapchainCreateInfoKHR t_CreateInfo{};
	t_CreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	t_CreateInfo.surface = r_Window.GetSurface();

	t_CreateInfo.minImageCount = t_ActualImageCount;
	t_CreateInfo.imageFormat = t_SurfaceFormat.format;
	t_CreateInfo.imageColorSpace = t_SurfaceFormat.colorSpace;
	t_CreateInfo.imageExtent = t_Extent;
	t_CreateInfo.imageArrayLayers = 1;
	t_CreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices t_Indices = FindQueueFamilies(p_VulkanDevice->m_PhysicalDevice, r_Window);
	uint32_t t_QueueFamilyIndices[] = { t_Indices.graphicsFamily.value(), t_Indices.presentFamily.value() };

	if (t_Indices.graphicsFamily != t_Indices.presentFamily)
	{
		t_CreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		t_CreateInfo.queueFamilyIndexCount = 2;
		t_CreateInfo.pQueueFamilyIndices = t_QueueFamilyIndices;
	}
	else
	{
		t_CreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		t_CreateInfo.queueFamilyIndexCount = 0; // Optional
		t_CreateInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	//Starting Transform for Objects such as a specific starting rotation, now just the standard 0 for all.
	t_CreateInfo.preTransform = t_SwapChainSupport.capabilities.currentTransform;

	//No Transparent Window.
	t_CreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	t_CreateInfo.presentMode = t_PresentMode;
	t_CreateInfo.clipped = VK_TRUE;

	//If a new SwapChain gets created such as resizing the window.
	t_CreateInfo.oldSwapchain = t_OldSwapChain;

	if (vkCreateSwapchainKHR(*p_VulkanDevice, &t_CreateInfo, nullptr, &SwapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create swap chain!");
	}

	if (t_OldSwapChain != VK_NULL_HANDLE)
	{
		for (size_t i = 0; i < SwapChainImageViews.size(); i++) {
			vkDestroyImageView(*p_VulkanDevice, SwapChainImageViews[i], nullptr);
		}

		vkDestroySwapchainKHR(*p_VulkanDevice, t_OldSwapChain, nullptr);
	}

	vkGetSwapchainImagesKHR(*p_VulkanDevice, SwapChain, &t_ActualImageCount, nullptr);
	
	SwapChainImages.resize(t_ActualImageCount);
	SwapChainImageViews.resize(t_ActualImageCount);
	SwapChainFrameBuffers.resize(t_ActualImageCount);

	vkGetSwapchainImagesKHR(*p_VulkanDevice, SwapChain, &t_ActualImageCount, SwapChainImages.data());

	//Setting the Local variables
	SwapChainImageFormat = t_SurfaceFormat.format;
	SwapChainExtent = t_Extent;
}

void VulkanSwapChain::CreateImageViews(ImageHandler* p_ImageHandler)
{
	for (uint32_t i = 0; i < SwapChainImageViews.size(); i++)
	{
		SwapChainImageViews[i] = p_ImageHandler->CreateImageView(SwapChainImages[i], SwapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	}
}

#pragma region Initializer Functions

VkSurfaceFormatKHR VulkanSwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& a_AvailableFormats)
{
	for (const auto& t_AvailableFormat : a_AvailableFormats)
	{
		if (t_AvailableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && t_AvailableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return t_AvailableFormat;
		}
	}

	return a_AvailableFormats[0];
}

VkPresentModeKHR VulkanSwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& a_AvailablePresentModes)
{
	for (const auto& t_AvailablePresentMode : a_AvailablePresentModes)
	{
		if (t_AvailablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return t_AvailablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanSwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& a_Capabilities, Window& r_Window)
{
	if (a_Capabilities.currentExtent.width != UINT32_MAX) {
		return a_Capabilities.currentExtent;
	}
	else {
		int t_Width, t_Height;
		glfwGetFramebufferSize(r_Window.GetWindow(), &t_Width, &t_Height);

		VkExtent2D t_ActualExtent = {
			static_cast<uint32_t>(t_Width),
			static_cast<uint32_t>(t_Height)
		};

		t_ActualExtent.width = std::clamp(t_ActualExtent.width, a_Capabilities.minImageExtent.width, a_Capabilities.maxImageExtent.width);
		t_ActualExtent.height = std::clamp(t_ActualExtent.height, a_Capabilities.minImageExtent.height, a_Capabilities.maxImageExtent.height);
		return t_ActualExtent;
	}
}

#pragma endregion