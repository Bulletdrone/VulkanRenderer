#define VK_USE_PLATFORM_WIN32_KHR
#include "Renderer.h"

#include <iostream>
#include <set>
#include <cstdint>
#include <algorithm>

Renderer::Renderer()
{
	//Setting up GLFW.
	m_Window = new Window(800, 600, "VulkanTest");

	if (DE_EnableValidationLayers)
		DE_VulkanDebug = new VulkanDebug(mvk_Instance);

	CreateVKInstance();
	
	if (DE_EnableValidationLayers)
		DE_VulkanDebug->SetupDebugMessanger(nullptr);

	m_Window->SetupVKWindow(mvk_Instance);
	VkPhysicalDevice physicalDevice = PickPhysicalDevice();
	//CreateLogicalDevice();
	m_VulkanDevice.VulkanDeviceSetup(physicalDevice, *m_Window, DE_VulkanDebug, FRAMEBUFFER_AMOUNT);
	m_VulkanDevice.CreateLogicalDevice(std::vector<const char*>(), *m_Window, mvk_GraphicsQueue, mvk_PresentQueue);
	
	SetupHandlers();
	CreateSwapChain();
	CreateImageViews();
	CreateRenderPass();

	m_ShaderManager = new ShaderManager(m_VulkanDevice, mvk_SwapChainExtent);
}

Renderer::~Renderer()
{
	vkDeviceWaitIdle(m_VulkanDevice);
	CleanupSwapChain();

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(m_VulkanDevice, mvk_RenderFinishedSemaphore[i], nullptr);
		vkDestroySemaphore(m_VulkanDevice, mvk_ImageAvailableSemaphore[i], nullptr);
		vkDestroyFence(m_VulkanDevice, mvk_InFlightFences[i], nullptr);
	}
	//delete m_CommandHandler;

	delete m_ShaderManager;
	vkDestroyDevice(m_VulkanDevice, nullptr);

	if (DE_EnableValidationLayers)
		delete DE_VulkanDebug;

	vkDestroySurfaceKHR(mvk_Instance, m_Window->GetSurface(), nullptr);
	vkDestroyInstance(mvk_Instance, nullptr);
	delete m_Window;
}

//Must be done after setting up the physical device.
void Renderer::SetupHandlers()
{
	m_ImageHandler = new ImageHandler(m_VulkanDevice);

	m_DepthHandler = new DepthHandler(m_VulkanDevice, m_ImageHandler);
}

//Call this after the creation of Vulkan.
void Renderer::SetupRenderObjects()
{
	m_VulkanDevice.CreateUniformBuffers(mvk_ViewProjectionBuffers, mvk_ViewProjectionBuffersMemory, mvk_SwapChainImages.size());

	CreateSyncObjects();
}

void Renderer::CleanupSwapChain()
{
	for (size_t i = 0; i < mvk_SwapChainFrameBuffers.size(); i++) {
		vkDestroyFramebuffer(m_VulkanDevice, mvk_SwapChainFrameBuffers[i], nullptr);
	}

	//m_VulkanDevice.FreeCommandPool();

	vkDestroyPipeline(m_VulkanDevice, mvk_Pipeline, nullptr);
	vkDestroyPipelineLayout(m_VulkanDevice, mvk_PipelineLayout, nullptr);
	vkDestroyRenderPass(m_VulkanDevice, mvk_RenderPass, nullptr);

	for (size_t i = 0; i < mvk_SwapChainImageViews.size(); i++) {
		vkDestroyImageView(m_VulkanDevice, mvk_SwapChainImageViews[i], nullptr);
	}

	vkDestroySwapchainKHR(m_VulkanDevice, mvk_SwapChain, nullptr);

	for (size_t i = 0; i < mvk_SwapChainImages.size(); i++)
	{
		vkDestroyBuffer(m_VulkanDevice, mvk_ViewProjectionBuffers[i], nullptr);
		vkFreeMemory(m_VulkanDevice, mvk_ViewProjectionBuffersMemory[i], nullptr);
	}
	
}

void Renderer::RecreateSwapChain()
{
	m_Window->CheckMinimized();

	vkDeviceWaitIdle(m_VulkanDevice);
		
	CleanupSwapChain();

	CreateSwapChain();
	CreateImageViews();
	CreateRenderPass();

	//Can be faulty, needs testing.
	m_ShaderManager->RecreatePipelines(mvk_RenderPass);
	CreateDepthResources();
	CreateFrameBuffers();

	SetupRenderObjects();
	m_ShaderManager->RecreateDescriptors(FRAMEBUFFER_AMOUNT, mvk_ViewProjectionBuffers);
}

void Renderer::CreateVKInstance()
{
	//If debug is enabled check if the layers are supported.
	if (DE_EnableValidationLayers && !DE_VulkanDebug->CheckValidationLayerSupport())
		throw std::runtime_error("validation layers requested, but not available!");


	//Setting up Vulkan: ApplicationInfo.
	VkApplicationInfo t_AppInfo{};
	t_AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	t_AppInfo.pApplicationName = "Zeep's VulkanTest";
	t_AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	t_AppInfo.pEngineName = "No Engine";
	t_AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	t_AppInfo.apiVersion = VK_API_VERSION_1_0;

	//Setting up Vulkan: VKInstance Setup
	VkInstanceCreateInfo t_CreateInfo{};
	t_CreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	t_CreateInfo.pApplicationInfo = &t_AppInfo;

	auto t_Extentions = m_Window->GetRequiredExtentions(DE_EnableValidationLayers);

	t_CreateInfo.enabledExtensionCount = static_cast<uint32_t>(t_Extentions.size());
	t_CreateInfo.ppEnabledExtensionNames = t_Extentions.data();

	//Set the Debug Layers if debug is enabled, otherwise set it to 0
	if (DE_EnableValidationLayers)
	{
		DE_VulkanDebug->AddDebugLayerInstance(t_CreateInfo);

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
		DE_VulkanDebug->PopulateDebugStartupMessengerCreateInfo(debugCreateInfo);
		t_CreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
	}
	else
	{
		t_CreateInfo.enabledLayerCount = 0;
		t_CreateInfo.pNext = nullptr;
	}

	//Create and check if the Instance is a success.
	if (vkCreateInstance(&t_CreateInfo, nullptr, &mvk_Instance) != VK_SUCCESS)
	{
		throw printf("Vulkan not successfully Instanced.");
	}
}

VkPhysicalDevice Renderer::PickPhysicalDevice()
{
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

	uint32_t t_DeviceCount = 0;
	vkEnumeratePhysicalDevices(mvk_Instance, &t_DeviceCount, nullptr);

	//No devices that support Vulcan.
	if (t_DeviceCount == 0) 
	{
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> t_Devices(t_DeviceCount);
	vkEnumeratePhysicalDevices(mvk_Instance, &t_DeviceCount, t_Devices.data());

	for (const VkPhysicalDevice& device : t_Devices) 
	{
		if (IsDeviceSuitable(device)) 
		{
			VkPhysicalDeviceProperties t_PhysDeviceProperties;
			vkGetPhysicalDeviceProperties(device, &t_PhysDeviceProperties);
			if (t_PhysDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			{
				physicalDevice = device;
				break;
			}
		}
	}

	if (physicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("failed to find a suitable GPU!");
	}

	return physicalDevice;
}

void Renderer::CreateSwapChain()
{
	SwapChainSupportDetails t_SwapChainSupport = QuerySwapChainSupport(m_VulkanDevice.m_PhysicalDevice);

	//Getting the needed values from the PhysicalDevice.
	VkSurfaceFormatKHR t_SurfaceFormat = ChooseSwapSurfaceFormat(t_SwapChainSupport.formats);
	VkPresentModeKHR t_PresentMode = ChooseSwapPresentMode(t_SwapChainSupport.presentModes);
	VkExtent2D t_Extent = ChooseSwapExtent(t_SwapChainSupport.capabilities);

	//Setting up the SwapChain.
	uint32_t t_ImageCount = t_SwapChainSupport.capabilities.minImageCount + 1;
	uint32_t t_ActualImageCount = FRAMEBUFFER_AMOUNT;

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
	t_CreateInfo.surface = m_Window->GetSurface();

	t_CreateInfo.minImageCount = t_ActualImageCount;
	t_CreateInfo.imageFormat = t_SurfaceFormat.format;
	t_CreateInfo.imageColorSpace = t_SurfaceFormat.colorSpace;
	t_CreateInfo.imageExtent = t_Extent;
	t_CreateInfo.imageArrayLayers = 1;
	t_CreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices t_Indices = FindQueueFamilies(m_VulkanDevice.m_PhysicalDevice);
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

	//If a new SwapChain gets created such as resizing the window, ignore for now.
	t_CreateInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(m_VulkanDevice, &t_CreateInfo, nullptr, &mvk_SwapChain) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(m_VulkanDevice, mvk_SwapChain, &t_ActualImageCount, nullptr);
	mvk_SwapChainImages.resize(t_ActualImageCount);
	vkGetSwapchainImagesKHR(m_VulkanDevice, mvk_SwapChain, &t_ActualImageCount, mvk_SwapChainImages.data());

	//Setting the Local variables
	mvk_SwapChainImageFormat = t_SurfaceFormat.format;
	mvk_SwapChainExtent = t_Extent;
}

void Renderer::CreateImageViews()
{
	mvk_SwapChainImageViews.resize(mvk_SwapChainImages.size());

	for (uint32_t i = 0; i < mvk_SwapChainImages.size(); i++) 
	{
		mvk_SwapChainImageViews[i] = m_ImageHandler->CreateImageView(mvk_SwapChainImages[i], mvk_SwapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	}
}

void Renderer::CreateRenderPass()
{
	//Color Attachment.
	VkAttachmentDescription t_ColorAttachment{};
	t_ColorAttachment.format = mvk_SwapChainImageFormat;
	t_ColorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	t_ColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	t_ColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	t_ColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	t_ColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	t_ColorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	t_ColorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference t_ColorAttachmentRef{};
	t_ColorAttachmentRef.attachment = 0;
	t_ColorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	//DEPTH BUFFER
	VkAttachmentDescription t_DepthAttachment{};
	t_DepthAttachment.format = m_DepthHandler->FindDepthFormat();
	t_DepthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	t_DepthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	t_DepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	t_DepthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	t_DepthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	t_DepthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	t_DepthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference t_DepthAttachmentRef{};
	t_DepthAttachmentRef.attachment = 1;
	t_DepthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription t_Subpass{};
	t_Subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

	t_Subpass.colorAttachmentCount = 1;
	t_Subpass.pColorAttachments = &t_ColorAttachmentRef;
	t_Subpass.pDepthStencilAttachment = &t_DepthAttachmentRef;

	VkSubpassDependency t_Dependency{};
	t_Dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	t_Dependency.dstSubpass = 0;

	t_Dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;;
	t_Dependency.srcAccessMask = 0;

	t_Dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;;
	t_Dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;;

	//Setting the struct.
	std::array<VkAttachmentDescription, 2> t_Attachments = { t_ColorAttachment, t_DepthAttachment };
	VkRenderPassCreateInfo t_RenderPassInfo{};
	t_RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	t_RenderPassInfo.attachmentCount = static_cast<uint32_t>(t_Attachments.size());;
	t_RenderPassInfo.pAttachments = t_Attachments.data();
	t_RenderPassInfo.subpassCount = 1;
	t_RenderPassInfo.pSubpasses = &t_Subpass;
	t_RenderPassInfo.dependencyCount = 1;
	t_RenderPassInfo.pDependencies = &t_Dependency;

	if (vkCreateRenderPass(m_VulkanDevice, &t_RenderPassInfo, nullptr, &mvk_RenderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}
}

uint32_t Renderer::CreateDescriptorLayout(TextureData* a_textureData)
{
	return m_ShaderManager->CreateDescriptorSetLayout(a_textureData);
}

uint32_t Renderer::CreateGraphicsPipeline(uint32_t a_DescID)
{
	return m_ShaderManager->CreatePipelineData(mvk_RenderPass, a_DescID);
}

void Renderer::CreateDescriptorPool(uint32_t a_DescID)
{
	m_ShaderManager->CreateDescriptorPool(mvk_SwapChainImageViews.size(), a_DescID);
}

void Renderer::CreateDescriptorSet(uint32_t a_DescID)
{
		m_ShaderManager->CreateDescriptorSet(mvk_SwapChainImageViews.size(), a_DescID, mvk_ViewProjectionBuffers);
}

void Renderer::CreateFrameBuffers()
{
	mvk_SwapChainFrameBuffers.resize(mvk_SwapChainImageViews.size());

	for (size_t i = 0; i < mvk_SwapChainImageViews.size(); i++)
	{
		std::array<VkImageView, 2> t_Attachments = {
		mvk_SwapChainImageViews[i],
		m_DepthHandler->GetDepthTest().depthImageView
		};

		VkFramebufferCreateInfo t_FramebufferInfo{};
		t_FramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		t_FramebufferInfo.renderPass = mvk_RenderPass;
		t_FramebufferInfo.attachmentCount = static_cast<uint32_t>(t_Attachments.size());
		t_FramebufferInfo.pAttachments = t_Attachments.data();
		t_FramebufferInfo.width = mvk_SwapChainExtent.width;
		t_FramebufferInfo.height = mvk_SwapChainExtent.height;
		t_FramebufferInfo.layers = 1;

		if (vkCreateFramebuffer(m_VulkanDevice, &t_FramebufferInfo, nullptr, &mvk_SwapChainFrameBuffers[i]) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void Renderer::CreateCommandPool()
{
	m_VulkanDevice.CreateCommandPools(FindQueueFamilies(m_VulkanDevice.m_PhysicalDevice));
}

void Renderer::CreateDepthResources()
{
	m_DepthHandler->CreateDepthResources(mvk_SwapChainExtent.width, mvk_SwapChainExtent.height);
}

void Renderer::CreateSyncObjects()
{
	mvk_ImageAvailableSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
	mvk_RenderFinishedSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
	mvk_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	mvk_ImagesInFlight.resize(mvk_SwapChainImages.size(), VK_NULL_HANDLE);

	VkSemaphoreCreateInfo t_SemaphoreInfo{};
	t_SemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo t_FenceInfo{};
	t_FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	t_FenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (vkCreateSemaphore(m_VulkanDevice, &t_SemaphoreInfo, nullptr, &mvk_ImageAvailableSemaphore[i]) != VK_SUCCESS ||
			vkCreateSemaphore(m_VulkanDevice, &t_SemaphoreInfo, nullptr, &mvk_RenderFinishedSemaphore[i]) != VK_SUCCESS ||
			vkCreateFence(m_VulkanDevice, &t_FenceInfo, nullptr, &mvk_InFlightFences[i]) != VK_SUCCESS)  
		{
			throw std::runtime_error("failed to create semaphores for a frame!");
		}
	}
}

void Renderer::SetupMesh(MeshData* a_MeshData)
{
	m_VulkanDevice.CreateVertexBuffers(a_MeshData->GetVertexData());
	m_VulkanDevice.CreateIndexBuffers(a_MeshData->GetIndexData());
}

void Renderer::SetupImage(TextureData& a_TextureData, const char* a_ImagePath)
{
	VkDeviceSize t_Size;
	int t_Width, t_Height, t_Channels;

	m_ImageHandler->CreateTextureImage(a_TextureData.textureImage, a_TextureData.textureImageMemory,
		a_ImagePath, t_Size, t_Width, t_Height, t_Channels);

	a_TextureData.textureImageView = m_ImageHandler->CreateImageView(a_TextureData.textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
	a_TextureData.textureSampler = m_ImageHandler->CreateTextureSampler();
}

void Renderer::DrawFrame(uint32_t& r_ImageIndex, float a_dt)
{
	bool recreateSwapChain = false;

	vkWaitForFences(m_VulkanDevice, 1, &mvk_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

	VkResult t_Result = vkAcquireNextImageKHR(m_VulkanDevice, mvk_SwapChain, UINT64_MAX, mvk_ImageAvailableSemaphore[m_CurrentFrame], VK_NULL_HANDLE, &r_ImageIndex);

	UpdateUniformBuffer(r_ImageIndex, a_dt);

	if (t_Result == VK_ERROR_OUT_OF_DATE_KHR || t_Result == VK_SUBOPTIMAL_KHR || m_Window->m_FrameBufferResized)
	{
		m_Window->m_FrameBufferResized = false;
		RecreateSwapChain();
		return;
	}
	else if (t_Result != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	// Check if a previous frame is using this image (i.e. there is its fence to wait on)
	if (mvk_ImagesInFlight[r_ImageIndex] != VK_NULL_HANDLE) 
	{
		vkWaitForFences(m_VulkanDevice, 1, &mvk_ImagesInFlight[r_ImageIndex], VK_TRUE, UINT64_MAX);
	}
	// Mark the image as now being in use by this frame
	mvk_ImagesInFlight[r_ImageIndex] = mvk_InFlightFences[m_CurrentFrame];

	//Draw the objects using a single command buffer.
	m_VulkanDevice.ClearPreviousCommand(m_CurrentFrame);

	VkCommandBuffer& t_MainBuffer = m_VulkanDevice.CreateAndBeginCommand(m_CurrentFrame, FindQueueFamilies(m_VulkanDevice.m_PhysicalDevice).graphicsFamily.value(),
		mvk_RenderPass, mvk_SwapChainFrameBuffers[m_CurrentFrame], mvk_SwapChainExtent);

	DrawObjects(t_MainBuffer);
	m_VulkanDevice.EndCommand(t_MainBuffer);


	VkSubmitInfo t_SubmitInfo{};
	t_SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore t_WaitSemaphores[] = { mvk_ImageAvailableSemaphore[m_CurrentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	t_SubmitInfo.waitSemaphoreCount = 1;
	t_SubmitInfo.pWaitSemaphores = t_WaitSemaphores;
	t_SubmitInfo.pWaitDstStageMask = waitStages;

	t_SubmitInfo.commandBufferCount = 1;
	t_SubmitInfo.pCommandBuffers = &t_MainBuffer;

	VkSemaphore t_SignalSemaphores[] = { mvk_RenderFinishedSemaphore[m_CurrentFrame] };
	t_SubmitInfo.signalSemaphoreCount = 1;
	t_SubmitInfo.pSignalSemaphores = t_SignalSemaphores;


	vkResetFences(m_VulkanDevice, 1, &mvk_InFlightFences[m_CurrentFrame]);
	if (vkQueueSubmit(mvk_GraphicsQueue, 1, &t_SubmitInfo, mvk_InFlightFences[m_CurrentFrame]) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR t_PresentInfo{};
	t_PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	t_PresentInfo.waitSemaphoreCount = 1;
	t_PresentInfo.pWaitSemaphores = t_SignalSemaphores;

	VkSwapchainKHR t_SwapChains[] = { mvk_SwapChain };
	t_PresentInfo.swapchainCount = 1;
	t_PresentInfo.pSwapchains = t_SwapChains;
	t_PresentInfo.pImageIndices = &r_ImageIndex;

	t_PresentInfo.pResults = nullptr; // Optional

	t_Result = vkQueuePresentKHR(mvk_PresentQueue, &t_PresentInfo);

	if (t_Result == VK_ERROR_OUT_OF_DATE_KHR || t_Result == VK_SUBOPTIMAL_KHR) {
		RecreateSwapChain();
	}
	else if (t_Result != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to present swap chain image!");
	}

	//vkQueueWaitIdle(m_VKPresentQueue);
	m_PreviousFrame = m_CurrentFrame;
	m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::DrawObjects(VkCommandBuffer& r_CmdBuffer)
{
	//Performance boost when getting the same data needed.
	MeshData* t_LastMeshData = nullptr;

	uint32_t t_LastPipelineID = 0;
	PipeLineData& t_CurrentPipeLine = m_ShaderManager->PipelinePool.Get(t_LastPipelineID);
	vkCmdBindPipeline(r_CmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, t_CurrentPipeLine.pipeLine);

	//Create this once for performance boost of not creating a mat4 everytime.
	InstanceModel t_PushInstance;

	for (int i = 0; i < p_RenderObjects->size(); i++)
	{
		BaseRenderObject* t_RenderObject = p_RenderObjects->at(i);
		uint32_t t_PipelineID = t_RenderObject->GetPipeLineID();

		//only bind the pipeline if it doesn't match with the already bound one
		if (t_PipelineID != t_LastPipelineID)
		{
			t_CurrentPipeLine = m_ShaderManager->PipelinePool.Get(t_PipelineID);
			t_LastPipelineID = t_PipelineID;
			vkCmdBindPipeline(r_CmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, t_CurrentPipeLine.pipeLine);
		}

		//Set Descriptor
		vkCmdBindDescriptorSets(r_CmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, t_CurrentPipeLine.pipeLineLayout, 0, 1, &t_CurrentPipeLine.p_DescriptorData->descriptorSets[m_CurrentFrame].at(0), 0, nullptr);

		//Set the Push constant data.
		t_PushInstance.model = t_RenderObject->GetModelMatrix();

		vkCmdPushConstants(r_CmdBuffer, t_CurrentPipeLine.pipeLineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(InstanceModel), &t_PushInstance);

		//only bind the mesh if it's a different one from last bind
		if (t_RenderObject->GetMeshData() != t_LastMeshData) {
			//bind the mesh vertex buffer with offset 0
			VkDeviceSize t_VertOffset = 0;
			//Set Vertex
			vkCmdBindVertexBuffers(r_CmdBuffer, 0, 1, &t_RenderObject->GetVertexData()->GetBuffer(), &t_VertOffset);

			//Set Indices
			vkCmdBindIndexBuffer(r_CmdBuffer, t_RenderObject->GetIndexData()->GetBuffer(), 0, VK_INDEX_TYPE_UINT16);

			t_LastMeshData = t_RenderObject->GetMeshData();
		}
		vkCmdDrawIndexed(r_CmdBuffer, static_cast<uint32_t>(t_RenderObject->GetIndexData()->GetElementCount()), 1, 0, 0, 0);
	}
}

void Renderer::UpdateUniformBuffer(uint32_t a_CurrentImage, float a_dt)
{
	ViewProjection ubo{};
	//ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), mvk_SwapChainExtent.width / (float)mvk_SwapChainExtent.height, 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;

	void* data;
	vkMapMemory(m_VulkanDevice, mvk_ViewProjectionBuffersMemory[a_CurrentImage], 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(m_VulkanDevice, mvk_ViewProjectionBuffersMemory[a_CurrentImage]);
}

QueueFamilyIndices Renderer::FindQueueFamilies(VkPhysicalDevice a_Device)
{
	QueueFamilyIndices t_Indices;

	uint32_t t_QueueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(a_Device, &t_QueueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> t_QueueFamilies(t_QueueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(a_Device, &t_QueueFamilyCount, t_QueueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : t_QueueFamilies) 
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
		{
			t_Indices.graphicsFamily = i;
			VkBool32 t_PresentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(a_Device, i, m_Window->GetSurface(), &t_PresentSupport);

			if (t_PresentSupport) 
			{
				t_Indices.presentFamily = i;
			}
		}

		if (t_Indices.isComplete())
			break;

		i++;
	}

	return t_Indices;
}

SwapChainSupportDetails Renderer::QuerySwapChainSupport(VkPhysicalDevice a_Device) {
	SwapChainSupportDetails t_Details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(a_Device, m_Window->GetSurface(), &t_Details.capabilities);

	//Get Format Data
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(a_Device, m_Window->GetSurface(), &formatCount, nullptr);

	if (formatCount != 0) 
	{
		t_Details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(a_Device, m_Window->GetSurface(), &formatCount, t_Details.formats.data());
	}

	//Get Present Data
	uint32_t t_PresentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(a_Device, m_Window->GetSurface(), &t_PresentModeCount, nullptr);

	if (t_PresentModeCount != 0) 
	{
		t_Details.presentModes.resize(t_PresentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(a_Device, m_Window->GetSurface(), &t_PresentModeCount, t_Details.presentModes.data());
	}

	return t_Details;
}

VkSurfaceFormatKHR Renderer::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& a_AvailableFormats) 
{
    for (const auto& t_AvailableFormat : a_AvailableFormats)
	{
        if (t_AvailableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && t_AvailableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return t_AvailableFormat;
        }
    }

    return a_AvailableFormats[0];
}

VkPresentModeKHR Renderer::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& a_AvailablePresentModes)
{
	for (const auto& t_AvailablePresentMode : a_AvailablePresentModes) 
	{
		if (t_AvailablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return t_AvailablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Renderer::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& a_Capabilities)
{
	if (a_Capabilities.currentExtent.width != UINT32_MAX) {
		return a_Capabilities.currentExtent;
	}
	else {
		int t_Width, t_Height;
		glfwGetFramebufferSize(m_Window->GetWindow(), &t_Width, &t_Height);

		VkExtent2D t_ActualExtent = {
			static_cast<uint32_t>(t_Width),
			static_cast<uint32_t>(t_Height)
		};

		t_ActualExtent.width = std::clamp(t_ActualExtent.width, a_Capabilities.minImageExtent.width, a_Capabilities.maxImageExtent.width);
		t_ActualExtent.height = std::clamp(t_ActualExtent.height, a_Capabilities.minImageExtent.height, a_Capabilities.maxImageExtent.height);
		return t_ActualExtent;
	}
}
	
bool Renderer::IsDeviceSuitable(VkPhysicalDevice a_Device)
{
	//Queues are supported.
	QueueFamilyIndices t_Indices = FindQueueFamilies(a_Device);

	//Extentions from m_DeviceExtensions are supported.
	bool t_ExtensionsSupported = CheckDeviceExtensionSupport(a_Device);

	//Swapchain is sufficiently supported.
	bool t_SwapChainAdequate = false;
	if (t_ExtensionsSupported) 
	{
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(a_Device);
		t_SwapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	VkPhysicalDeviceFeatures t_SupportedFeatures;
	vkGetPhysicalDeviceFeatures(a_Device, &t_SupportedFeatures);

	//The device works with everything that the program needs.
	return t_Indices.isComplete() && t_ExtensionsSupported && t_SwapChainAdequate && t_SupportedFeatures.samplerAnisotropy;;
}

bool Renderer::CheckDeviceExtensionSupport(VkPhysicalDevice a_Device)
{
	uint32_t t_ExtensionCount;
	vkEnumerateDeviceExtensionProperties(a_Device, nullptr, &t_ExtensionCount, nullptr);

	std::vector<VkExtensionProperties> t_AvailableExtensions(t_ExtensionCount);
	vkEnumerateDeviceExtensionProperties(a_Device, nullptr, &t_ExtensionCount, t_AvailableExtensions.data());

	std::set<std::string> t_RequiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

	for (const auto& extension : t_AvailableExtensions) 
	{
		t_RequiredExtensions.erase(extension.extensionName);
	}

	return t_RequiredExtensions.empty();
}