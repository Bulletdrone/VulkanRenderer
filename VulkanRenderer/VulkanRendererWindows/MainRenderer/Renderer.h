#pragma once

#include "Window.h"

#include "ShaderManager.h"
#include "VulkanDebugger/VulkanDebug.h"

#include "MeshData.h"

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


constexpr int MAX_FRAMES_IN_FLIGHT = 2;

class Renderer
{
public:
	Renderer();
	~Renderer();

	void CleanupSwapChain();
	void RecreateSwapChain();

	void CreateVKInstance();
	void PickPhysicalDevice();
	void CreateLogicalDevice();
	void CreateSwapChain();
	void CreateImageViews();
	void CreateRenderPass();

	void CreateFrameBuffers();
	void CreateCommandPool();
	void CreateCommandBuffers();

	void CreateSyncObjects();

	//BufferData
	void SetupMesh(MeshData* a_MeshData);
	void CreateBufferFromMesh(MeshData* a_MeshData, VkDeviceSize a_Size, VkBufferUsageFlags a_Usage, VkMemoryPropertyFlags a_Properties, VkBuffer& r_Buffer, VkDeviceMemory& r_BufferMemory);
	void CopyBufferFromMesh(MeshData* a_MeshData, VkDeviceSize a_Size, VkBuffer& r_Buffer);

	void DrawFrame(uint32_t& r_ImageIndex);

	//QueueFamily
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice a_Device);

	//SwapChain Setup
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice a_Device);
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& a_AvailableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& a_AvailablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& a_Capabilities);

	bool CheckDeviceExtensionSupport(VkPhysicalDevice a_Device);
	uint32_t FindMemoryType(uint32_t a_TypeFilter, VkMemoryPropertyFlags a_Properties, VkPhysicalDevice& r_PhysDevice);



	//Getters
	GLFWwindow* GetWindow() const { return m_Window->GetWindow(); }


private:
	bool IsDeviceSuitable(VkPhysicalDevice a_Device);

	MeshData* TEMPMESH;

	//Window Data.
	Window* m_Window;

	//ShaderData
	ShaderManager* m_ShaderManager;

	std::vector<VkSemaphore> mvk_ImageAvailableSemaphore;
	std::vector<VkSemaphore> mvk_RenderFinishedSemaphore;
	std::vector<VkFence> mvk_InFlightFences;
	std::vector<VkFence> mvk_ImagesInFlight;
	size_t m_CurrentFrame = 0;

	//Primary Vulkan Data;
	VkInstance mvk_Instance;
	VkPhysicalDevice mvk_PhysicalDevice = VK_NULL_HANDLE;
	VkDevice mvk_Device;

	VkQueue mvk_GraphicsQueue;
	VkQueue mvk_PresentQueue;

	//The SwapChain for buffering images and more.
	VkSwapchainKHR mvk_SwapChain;
	std::vector<VkImage> mvk_SwapChainImages;
	std::vector<VkImageView> mvk_SwapChainImageViews;
	std::vector<VkFramebuffer> mvk_SwapChainFrameBuffers;

	//Buffer Commands
	VkCommandPool mvk_CommandPool;
	std::vector<VkCommandBuffer> mvk_CommandBuffers;

	//The RenderPipeline.
	VkRenderPass mvk_RenderPass;
	VkPipelineLayout mvk_PipelineLayout;
	VkPipeline mvk_Pipeline;

	VkFormat mvk_SwapChainImageFormat;
	VkExtent2D mvk_SwapChainExtent;

	const std::vector<const char*> m_DeviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	//DEBUGGING
	VulkanDebug* DE_VulkanDebug = nullptr;
#ifdef NDEBUG
	const bool DE_EnableValidationLayers = false;
#else
	const bool DE_EnableValidationLayers = true;
#endif
};