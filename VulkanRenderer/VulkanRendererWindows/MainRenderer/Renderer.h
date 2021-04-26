#pragma once

#include "Window.h"

#include "ShaderManager.h"
#include "VulkanDebugger/VulkanDebug.h"

#include <optional>

//Special struct for the Family Queue.
struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};


struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};


class Renderer
{
public:
	Renderer();
	~Renderer();

	void CreateVKInstance();
	void PickPhysicalDevice();
	void CreateLogicalDevice();
	void CreateSwapChain();
	void CreateImageViews();
	void CreateRenderPass();
	void CreateGraphicsPipeline();	

	void CreateFrameBuffer();
	void CreateCommandPool();
	void CreateCommandBuffers();

	//QueueFamily
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice a_Device);

	//SwapChain Setup
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice a_Device);
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& a_AvailableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& a_AvailablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& a_Capabilities);

	bool CheckDeviceExtensionSupport(VkPhysicalDevice a_Device);

	//Getters
	GLFWwindow* GetWindow() const { return m_Window->GetWindow(); }

private:
	bool IsDeviceSuitable(VkPhysicalDevice a_Device);

	//Window Data.
	Window* m_Window;

	//ShaderData
	ShaderManager* m_ShaderManager;

	//Primary Vulkan Data;
	VkInstance m_VKInstance;
	VkPhysicalDevice m_VKPhysicalDevice = VK_NULL_HANDLE;
	VkDevice m_VKDevice;

	VkQueue m_VKGraphicsQueue;
	VkQueue m_VKPresentQueue;

	//The SwapChain for buffering images and more.
	VkSwapchainKHR m_VKSwapChain;
	std::vector<VkImage> m_VKSwapChainImages;
	std::vector<VkImageView> m_VKSwapChainImageViews;
	std::vector<VkFramebuffer> m_VKSwapChainFrameBuffers;

	//Buffer Commands
	VkCommandPool m_VKCommandPool;
	std::vector<VkCommandBuffer> m_VKCommandBuffers;

	//The RenderPipeline.
	VkRenderPass m_VKRenderPass;
	VkPipelineLayout m_VKPipelineLayout;
	VkPipeline m_VKPipeline;

	VkFormat m_VKSwapChainImageFormat;
	VkExtent2D m_VKSwapChainExtent;

	const std::vector<const char*> m_DeviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	//DEBUGGING
	VulkanDebug* DE_VKDebug = nullptr;
#ifdef NDEBUG
	const bool DE_EnableValidationLayers = false;
#else
	const bool DE_EnableValidationLayers = true;
#endif
};