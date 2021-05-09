#pragma once

#include "Window.h"

#include "ShaderManager.h"
#include "VulkanDebugger/VulkanDebug.h"

#include "RenderObjects/BaseRenderObject.h"

//Handlers
#include "Handlers/BufferHandler.h"
#include "Handlers/CommandHandler.h"

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

	//Setting up that is done later.
	void SetupRenderObjects();

	void CleanupSwapChain();
	void RecreateSwapChain();

	void CreateVKInstance();
	void PickPhysicalDevice();
	void CreateLogicalDevice();
	void CreateSwapChain();
	void CreateImageViews();
	void CreateRenderPass();

	void CreateDescriptorPool();
	void CreateDescriptorSets();

	void CreateFrameBuffers();
	void CreateCommandPool();

	//BufferObject Creation.
	void CreateCommandBuffers();
	void CreateSyncObjects();

	//BufferData
	void SetupMesh(MeshData* a_MeshData);
	void SetupImage(TextureData& a_TextureData, const char* a_ImagePath);

	void DrawFrame(uint32_t& r_ImageIndex, float a_dt);
	void UpdateUniformBuffer(uint32_t a_CurrentImage, float a_dt);

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
	VkDevice& GetLogicalDevice() { return mvk_Device; }
	VkPhysicalDevice& GetPhysicalDevice() { return mvk_PhysicalDevice; }

	//Set the mesh vector pointer in the Renderer from the one in ObjectManager.
	void SetRenderObjectsVector(std::vector<BaseRenderObject*>* a_RenderObjects);

private:
	bool IsDeviceSuitable(VkPhysicalDevice a_Device);

	//All the renderObjects in ObjectManager.
	std::vector<BaseRenderObject*>* p_RenderObjects;

	//Window Data.
	Window* m_Window;

	//Handlers
	BufferHandler* m_BufferHandler;
	CommandHandler* m_CommandHandler;

	//ShaderData
	ShaderManager* m_ShaderManager;
	std::vector<VkBuffer> mvk_ViewProjectionBuffers;
	std::vector<VkDeviceMemory> mvk_ViewProjectionBuffersMemory;

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

	VkDescriptorSetLayout mvk_DescriptorSetLayout;
	VkDescriptorPool mvk_DescriptorPool;
	std::vector<VkDescriptorSet> mvk_DescriptorSets;

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

//SetRenderObjects from ObjectManager.
inline void Renderer::SetRenderObjectsVector(std::vector<BaseRenderObject*>* a_RenderObjects)
{
	p_RenderObjects = a_RenderObjects;
}