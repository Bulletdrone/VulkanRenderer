#pragma once

#include "Window.h"

#include "ShaderManager.h"
#include "VulkanDebugger/VulkanDebug.h"

#include <RenderObjects/BaseRenderObject.h>
#include <RenderObjects/CameraObject.h>

#include "Structs/FrameData.h"
#include "VulkanSwapChain.h"

//Handlers
#include "Handlers/ImageHandler.h"
#include "Handlers/DepthHandler.h"

constexpr uint32_t FRAMEBUFFER_AMOUNT = 2;
constexpr int MAX_FRAMES_IN_FLIGHT = 2;

struct Texture;

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
	void CreateRenderPass();

	uint32_t CreateGraphicsPipeline(std::vector<VkDescriptorSetLayout>& a_DescriptorSetLayouts);

	void CreateFrameBuffers();
	void CreateCommandPool();

	void CreateDepthResources();

	//BufferObject Creation.
	void CreateSyncObjects();

	//Set a new camera which the buffers get the viewprojection from.
	void ReplaceActiveCamera(CameraObject* a_Cam);

	//BufferData
	void SetupMesh(MeshData* a_MeshData);
	void SetupImage(Texture& a_Texture);

	void DrawFrame(uint32_t& r_ImageIndex);
	void DrawObjects(VkCommandBuffer& r_CmdBuffer);
	void UpdateUniformBuffer(uint32_t a_CurrentImage);

	bool CheckDeviceExtensionSupport(VkPhysicalDevice a_Device);

	//Getters
	GLFWwindow* GetWindow() const { return m_Window->GetWindow(); }
	VulkanDevice& GetVulkanDevice() { return m_VulkanDevice; }
	VulkanSwapChain& GetVulkanSwapChain() { return m_VulkanSwapChain; }
	VkInstance& GetInstance() { return mvk_Instance; }
	VkQueue& GetQueue() { return mvk_GraphicsQueue; }
	VkRenderPass& GetRenderPass() { return mvk_RenderPass; }

	float GetAspectRatio() { return m_VulkanSwapChain.SwapChainExtent.width / (float)m_VulkanSwapChain.SwapChainExtent.height; }

	//Set the mesh vector pointer in the Renderer from the one in ObjectManager.
	void SetRenderObjectsVector(std::vector<BaseRenderObject*>* a_RenderObjects);


	std::vector<VkBuffer> mvk_ViewProjectionBuffers;
	std::vector<VkDeviceMemory> mvk_ViewProjectionBuffersMemory;
	VkDescriptorSet GlobalSet[2];

private:
	bool IsDeviceSuitable(VkPhysicalDevice a_Device);

	//All the renderObjects in ObjectManager.
	std::vector<BaseRenderObject*>* p_RenderObjects;

	//Window Data.
	Window* m_Window;

	//Handlers
	ImageHandler* m_ImageHandler;
	DepthHandler* m_DepthHandler;

	//ShaderData
	ShaderManager* m_ShaderManager;


	CameraObject* p_ActiveCamera;

	std::vector<FrameData> m_FrameData;

	int m_CurrentFrame = 0;
	int m_PreviousFrame = -1;

	//Primary Vulkan Data;
	VkInstance mvk_Instance;

	VulkanDevice m_VulkanDevice;
	VulkanSwapChain m_VulkanSwapChain;

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
