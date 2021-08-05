#pragma once

#include "Window.h"

#include "ShaderManager.h"
#include "VulkanDebugger/VulkanDebug.h"
#include <RenderObjects/BaseRenderObject.h>

#include "Structs/FrameData.h"
#include "VulkanSwapChain.h"

//Handlers
#include "Handlers/ImageHandler.h"
#include "Handlers/DepthHandler.h"

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

constexpr uint32_t FRAMEBUFFER_AMOUNT = 2;
constexpr int MAX_FRAMES_IN_FLIGHT = 2;

class Renderer
{
public:
	Renderer();
	~Renderer();

	//Setting up that is done later.
	void SetupRenderObjects();
	void SetupHandlers();

	void CleanupSwapChain();
	void RecreateSwapChain();

	void CreateVKInstance();
	VkPhysicalDevice PickPhysicalDevice();
	void CreateSwapChain();
	void CreateImageViews();
	void CreateRenderPass();

	uint32_t CreateDescriptorLayout(TextureData* a_textureData);
	uint32_t CreateGraphicsPipeline(uint32_t a_DescID);
	void CreateDescriptorPool(uint32_t a_DescID);
	void CreateDescriptorSet(uint32_t a_DescID);

	void CreateFrameBuffers();
	void CreateCommandPool();

	void CreateDepthResources();

	//BufferObject Creation.
	void CreateSyncObjects();

	//BufferData
	void SetupMesh(MeshData* a_MeshData);
	void SetupImage(TextureData& a_TextureData, const char* a_ImagePath);

	void DrawFrame(uint32_t& r_ImageIndex, float a_dt);
	void DrawObjects(VkCommandBuffer& r_CmdBuffer);
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
	VulkanDevice& GetVulkanDevice() { return m_VulkanDevice; }

	//Set the mesh vector pointer in the Renderer from the one in ObjectManager.
	void SetRenderObjectsVector(std::vector<BaseRenderObject*>* a_RenderObjects);
	void SetTextureDataVector(std::vector<TextureData>* a_Textures);

private:
	bool IsDeviceSuitable(VkPhysicalDevice a_Device);

	//All the renderObjects in ObjectManager.
	std::vector<BaseRenderObject*>* p_RenderObjects;
	std::vector<TextureData>* p_Textures;

	//Window Data.
	Window* m_Window;

	//Handlers
	ImageHandler* m_ImageHandler;
	DepthHandler* m_DepthHandler;

	//ShaderData
	ShaderManager* m_ShaderManager;

	std::vector<VkBuffer> mvk_ViewProjectionBuffers;
	std::vector<VkDeviceMemory> mvk_ViewProjectionBuffersMemory;

	std::vector<FrameData> m_FrameData;

	size_t m_CurrentFrame = 0;
	size_t m_PreviousFrame = -1;

	//Primary Vulkan Data;
	VkInstance mvk_Instance;

	VulkanDevice m_VulkanDevice;
	VulkanSwapChain* m_VulkanSwapChain;

	VkQueue mvk_GraphicsQueue;
	VkQueue mvk_PresentQueue;

	//The RenderPipeline.
	VkRenderPass mvk_RenderPass;

	VkPipelineLayout mvk_PipelineLayout;
	VkPipeline mvk_Pipeline;

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

//SetTextureData from ObjectManager.
inline void Renderer::SetTextureDataVector(std::vector<TextureData>* a_Textures)
{
	p_Textures = a_Textures;
}
