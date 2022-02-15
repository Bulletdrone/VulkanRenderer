#define VK_USE_PLATFORM_WIN32_KHR
#include "Renderer.h"

#include <iostream>
#include <set>
#include <cstdint>
#include <algorithm>

#pragma warning (push, 0)
#include <imGUI/Backends/imgui_impl_vulkan.h>
#pragma warning (pop)
#include <GUI/GUI.h>


#include "Structs/Texture.h"
#include "Structs/Shader.h"
#include "Tools/VulkanInitializers.h"

Renderer::Renderer()
{
	m_FrameData.resize(FRAMEBUFFER_AMOUNT);
	//Setting up GLFW.
	m_Window = new Window(800, 600, "VulkanTest");

	if (DE_EnableValidationLayers)
		DE_VulkanDebug = new VulkanDebug(mvk_Instance);

	CreateVKInstance();
	
	if (DE_EnableValidationLayers)
		DE_VulkanDebug->SetupDebugMessanger(nullptr);

	m_Window->SetupVKWindow(mvk_Instance);
	VkPhysicalDevice physicalDevice = PickPhysicalDevice();

	m_VulkanDevice.VulkanDeviceSetup(physicalDevice, DE_VulkanDebug, FRAMEBUFFER_AMOUNT);
	m_VulkanDevice.CreateLogicalDevice(std::vector<const char*>(), *m_Window, mvk_GraphicsQueue, mvk_PresentQueue);
	
	SetupHandlers();
	m_VulkanSwapChain.ConnectVulkanDevice(&m_VulkanDevice);
	m_VulkanSwapChain.CreateSwapChain(*m_Window, static_cast<uint32_t>(m_FrameData.size()));
	m_VulkanSwapChain.CreateImageViews(m_ImageHandler);
	CreateRenderPass();

	m_ShaderManager = new ShaderManager(m_VulkanDevice, m_VulkanSwapChain.SwapChainExtent);
}

Renderer::~Renderer()
{
	for (size_t i = 0; i < m_FrameData.size(); i++) {
		vkDestroySemaphore(m_VulkanDevice, m_FrameData[i].RenderFinishedSemaphore, nullptr);
		vkDestroySemaphore(m_VulkanDevice, m_FrameData[i].ImageAvailableSemaphore, nullptr);
		vkDestroyFence(m_VulkanDevice, m_FrameData[i].InFlightFence, nullptr);
	}
	//delete m_CommandHandler;

	delete m_ShaderManager;
	m_VulkanDevice.CleanupDevice();

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

	m_DescriptorAllocator = new DescriptorAllocator();
	m_DescriptorAllocator->Init(m_VulkanDevice);

	m_DescriptorLayoutCache = new DescriptorLayoutCache();
	m_DescriptorLayoutCache->Init(m_VulkanDevice);
}

//Call this after the creation of Vulkan.
void Renderer::CreateStartBuffers()
{
	mvk_ViewProjectionBuffers.resize(FRAMEBUFFER_AMOUNT);
	mvk_ViewProjectionBuffersMemory.resize(FRAMEBUFFER_AMOUNT);

	for (uint32_t i = 0; i < FRAMEBUFFER_AMOUNT; i++)
	{
		m_VulkanDevice.CreateUniformBuffers(mvk_ViewProjectionBuffers[i], mvk_ViewProjectionBuffersMemory[i], sizeof(ViewProjection));
	}
	CreateGlobalDescriptor();

	CreateSyncObjects();
}

void Renderer::CleanupSwapChain()
{
	vkDeviceWaitIdle(m_VulkanDevice);
	for (size_t i = 0; i < m_FrameData.size(); i++) {
		vkDestroyFramebuffer(m_VulkanDevice, m_VulkanSwapChain.SwapChainFrameBuffers[i], nullptr);
	}

	//m_VulkanDevice.FreeCommandPool();

	vkDestroyPipeline(m_VulkanDevice, mvk_Pipeline, nullptr);
	vkDestroyPipelineLayout(m_VulkanDevice, mvk_PipelineLayout, nullptr);
	vkDestroyRenderPass(m_VulkanDevice, mvk_RenderPass, nullptr);

	for (size_t i = 0; i < m_FrameData.size(); i++)
	{
		vkDestroyBuffer(m_VulkanDevice, mvk_ViewProjectionBuffers[i], nullptr);
		vkFreeMemory(m_VulkanDevice, mvk_ViewProjectionBuffersMemory[i], nullptr);
	}
	
	m_DepthHandler->CleanupDepthTest();
}


void Renderer::RecreateSwapChain()
{
	m_Window->CheckMinimized();

	vkDeviceWaitIdle(m_VulkanDevice);
		
	CleanupSwapChain();

	m_VulkanSwapChain.CreateSwapChain(*m_Window, static_cast<uint32_t>(m_FrameData.size()));
	m_VulkanSwapChain.CreateImageViews(m_ImageHandler);
	CreateRenderPass();

	//Can be faulty, needs testing.
	//m_ShaderManager->RecreatePipelines(mvk_RenderPass);
	CreateDepthResources();
	CreateFrameBuffers();

	CreateStartBuffers();
	//m_ShaderManager->RecreateDescriptors(FRAMEBUFFER_AMOUNT, mvk_ViewProjectionBuffers, sizeof(ViewProjection));
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

void Renderer::CreateRenderPass()
{
	//Color Attachment.
	VkAttachmentDescription t_ColorAttachment{};
	t_ColorAttachment.format = m_VulkanSwapChain.SwapChainImageFormat;
	t_ColorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	t_ColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	t_ColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	t_ColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	t_ColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	t_ColorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	t_ColorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	//t_ColorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

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

void Renderer::CreateFrameBuffers()
{
	for (size_t i = 0; i < m_FrameData.size(); i++)
	{
		std::array<VkImageView, 2> t_Attachments = {
		m_VulkanSwapChain.SwapChainImageViews[i],
		m_DepthHandler->GetDepthTest().depthImageView
		};

		VkFramebufferCreateInfo t_FramebufferInfo = VkInit::FramebufferCreateInfo();
		t_FramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		t_FramebufferInfo.renderPass = mvk_RenderPass;
		t_FramebufferInfo.attachmentCount = static_cast<uint32_t>(t_Attachments.size());
		t_FramebufferInfo.pAttachments = t_Attachments.data();
		t_FramebufferInfo.width = m_VulkanSwapChain.SwapChainExtent.width;
		t_FramebufferInfo.height = m_VulkanSwapChain.SwapChainExtent.height;
		t_FramebufferInfo.layers = 1;

		if (vkCreateFramebuffer(m_VulkanDevice, &t_FramebufferInfo, nullptr, &m_VulkanSwapChain.SwapChainFrameBuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void Renderer::CreateCommandPool()
{
	m_VulkanDevice.CreateCommandPools(FindQueueFamilies(m_VulkanDevice.m_PhysicalDevice, *m_Window));
}

void Renderer::CreateDepthResources()
{
	m_DepthHandler->CreateDepthResources(m_VulkanSwapChain.SwapChainExtent.width, m_VulkanSwapChain.SwapChainExtent.height);
}

void Renderer::CreateSyncObjects()
{
	//mvk_ImageAvailableSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
	//mvk_RenderFinishedSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
	//mvk_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	//mvk_ImagesInFlight.resize(mvk_SwapChainImages.size(), VK_NULL_HANDLE);

	VkSemaphoreCreateInfo t_SemaphoreInfo{};
	t_SemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo t_FenceInfo{};
	t_FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	t_FenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < m_FrameData.size(); i++)
	{
		if (vkCreateSemaphore(m_VulkanDevice, &t_SemaphoreInfo, nullptr, &m_FrameData[i].ImageAvailableSemaphore) != VK_SUCCESS ||
			vkCreateSemaphore(m_VulkanDevice, &t_SemaphoreInfo, nullptr, &m_FrameData[i].RenderFinishedSemaphore) != VK_SUCCESS ||
			vkCreateFence(m_VulkanDevice, &t_FenceInfo, nullptr, &m_FrameData[i].InFlightFence) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create semaphores for a frame!");
		}
	}
}

void Renderer::ReplaceActiveCamera(CameraObject* a_Cam)
{
	p_ActiveCamera = a_Cam;
}

MeshHandle Renderer::GenerateMesh(const std::vector<Vertex>& a_Vertices, const std::vector<uint32_t>& a_Indices)
{
	MeshHandle t_Handle;
	MeshData& mesh = m_MeshPool.GetEmptyObject(t_Handle);

	mesh.vertices.elementSize = static_cast<uint32_t>(a_Vertices.size());
	mesh.indices.elementSize = static_cast<uint32_t>(a_Indices.size());

	m_VulkanDevice.CreateVertexBuffers(mesh.vertices, a_Vertices);
	m_VulkanDevice.CreateIndexBuffers(mesh.indices, a_Indices);

	return t_Handle;
}

void Renderer::SetupImage(Texture& a_Texture, const unsigned char* a_ImageBuffer)
{
	m_ImageHandler->CreateTextureImage(a_Texture, a_ImageBuffer);

	a_Texture.textureImageView = m_ImageHandler->CreateImageView(a_Texture.textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
	a_Texture.textureSampler = m_ImageHandler->CreateTextureSampler();
}

ShaderHandle Renderer::CreateShader(const unsigned char* a_ShaderCode, const size_t a_CodeSize)
{
	VkShaderModuleCreateInfo t_CreateInfo = VkInit::ShaderModuleCreateInfo(a_ShaderCode, a_CodeSize);

	ShaderHandle t_Handle;
	Shader& t_Shader = m_ShaderPool.GetEmptyObject(t_Handle);
	if (vkCreateShaderModule(m_VulkanDevice, &t_CreateInfo, nullptr, &t_Shader.shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

	return t_Handle;
}

MaterialHandle Renderer::CreateMaterial(ShaderHandle a_Vert, ShaderHandle a_Frag, uint32_t a_UniCount, VkBuffer* a_UniBuffers, uint32_t a_ImageCount, Texture* a_Images, glm::vec4 a_Color)
{
	MaterialHandle t_Handle;
	Material& material = m_MaterialPool.GetEmptyObject(t_Handle);

	std::vector<VkDescriptorSetLayout> t_DescriptorLayouts{ m_GlobalSetLayout };

	material.ambientColor = a_Color;

	if (a_ImageCount != 0)
	{
		VkDescriptorSetLayoutBinding t_ImageBinding = VkInit::DescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, MATERIALBINDING);
		VkDescriptorSetLayoutCreateInfo t_ImageLayoutInfo = VkInit::DescriptorSetLayoutCreateInfo(1, &t_ImageBinding);
		VkDescriptorSetLayout t_ImageLayout = m_DescriptorLayoutCache->CreateLayout(&t_ImageLayoutInfo);

		t_DescriptorLayouts.push_back(t_ImageLayout);

		material.pipelineID = m_ShaderManager->CreatePipelineData(m_ShaderPool.Get(a_Vert), m_ShaderPool.Get(a_Frag), mvk_RenderPass, t_DescriptorLayouts);

		//Building descriptor sets.
		DescriptorBuilder t_Builder{};

		VkDescriptorImageInfo* t_Images = new VkDescriptorImageInfo[a_ImageCount];
		for (uint32_t i = 0; i < a_ImageCount; i++)
		{
			t_Images[i] = VkInit::DescriptorImageInfo(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, a_Images[i].textureImageView, a_Images[i].textureSampler);
		}

		t_Builder = DescriptorBuilder::Begin(m_DescriptorLayoutCache, m_DescriptorAllocator);
		t_Builder.BindImage(a_ImageCount, t_Images, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		t_Builder.Build(material.secondDescriptorSet, t_ImageLayout);


		delete[] t_Images;
		return t_Handle;
	}



	material.pipelineID = m_ShaderManager->CreatePipelineData(m_ShaderPool.Get(a_Vert), m_ShaderPool.Get(a_Frag), mvk_RenderPass, t_DescriptorLayouts);


	return t_Handle;
}

RenderObject Renderer::CreateRenderObject(Engine::Transform* a_Transform, MaterialHandle a_MaterialHandle, MeshHandle a_MeshHandle)
{
	RenderObject rendObj{};
	rendObj.p_Transform = a_Transform;
	rendObj.materialHandle = a_MaterialHandle;
	rendObj.meshHandle = a_MeshHandle;

	rendObj.IsValid();

	m_RenderObjects.push_back(rendObj);

	return rendObj;
}

RenderObject Renderer::CreateRenderObject(Engine::Transform* a_Transform, GeometryType a_Type, MaterialHandle a_MaterialHandle)
{
	if (a_MaterialHandle != RENDER_NULL_HANDLE)
		return CreateRenderObject(a_Transform, a_MaterialHandle, m_GeometryFactory.GetShape(a_Type));

	return CreateRenderObject(a_Transform, m_GeometryFactory.GetDebugMaterial(), m_GeometryFactory.GetShape(a_Type));
}

void Renderer::SetupGUIsystem(GUI::GUISystem* p_GuiSystem)
{
	p_GuiSystem->Init(m_VulkanDevice, mvk_Instance, m_VulkanDevice.m_PhysicalDevice, mvk_GraphicsQueue, mvk_RenderPass);
	
	//Setup standard font for imGUI.
	VkCommandBuffer t_Cmd = m_VulkanDevice.BeginSingleTimeCommands();
	ImGui_ImplVulkan_CreateFontsTexture(t_Cmd);
	m_VulkanDevice.EndSingleTimeCommands(t_Cmd);
}

void Renderer::SetupGeometryFactory()
{
	m_GeometryFactory.Init(this);
}

void Renderer::CreateGlobalDescriptor()
{
	DescriptorBuilder t_Builder{};

	VkDescriptorSetLayoutBinding t_CameraBinding = VkInit::DescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0);
	VkDescriptorSetLayoutCreateInfo t_CameraLayoutInfo = VkInit::DescriptorSetLayoutCreateInfo(1, &t_CameraBinding);
	m_GlobalSetLayout = m_DescriptorLayoutCache->CreateLayout(&t_CameraLayoutInfo);

	VkDescriptorBufferInfo buffer1 = VkInit::DescriptorBufferInfo(mvk_ViewProjectionBuffers[0], 0, sizeof(ViewProjection));
	t_Builder = DescriptorBuilder::Begin(m_DescriptorLayoutCache, m_DescriptorAllocator);
	t_Builder.BindBuffer(0, &buffer1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	t_Builder.Build(m_GlobalSet[0], m_GlobalSetLayout);

	VkDescriptorBufferInfo buffer2 = VkInit::DescriptorBufferInfo(mvk_ViewProjectionBuffers[1], 0, sizeof(ViewProjection));
	t_Builder = DescriptorBuilder::Begin(m_DescriptorLayoutCache, m_DescriptorAllocator);
	t_Builder.BindBuffer(0, &buffer2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	t_Builder.Build(m_GlobalSet[1], m_GlobalSetLayout);
}

void Renderer::DrawFrame()
{
	FrameData& frameData = m_FrameData[m_CurrentFrame];

	vkWaitForFences(m_VulkanDevice, 1, &frameData.InFlightFence, VK_TRUE, UINT64_MAX);

	uint32_t t_ImageIndex = 0;
	VkResult t_Result = vkAcquireNextImageKHR(m_VulkanDevice, m_VulkanSwapChain.SwapChain, UINT64_MAX, frameData.ImageAvailableSemaphore, VK_NULL_HANDLE, &t_ImageIndex);
	UpdateUniformBuffer(t_ImageIndex);

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
	if (frameData.ImageInFlight != VK_NULL_HANDLE)
	{
		vkWaitForFences(m_VulkanDevice, 1, &frameData.ImageInFlight, VK_TRUE, UINT64_MAX);
	}
	// Mark the image as now being in use by this frame
	frameData.ImageInFlight = frameData.InFlightFence;

	//Draw the objects using a single command buffer.
	m_VulkanDevice.ClearPreviousCommand(m_CurrentFrame);

	VkCommandBuffer t_MainBuffer = m_VulkanDevice.CreateAndBeginCommand(m_CurrentFrame, FindQueueFamilies(m_VulkanDevice.m_PhysicalDevice, *m_Window).graphicsFamily.value(),
		mvk_RenderPass, m_VulkanSwapChain.SwapChainFrameBuffers[m_CurrentFrame], m_VulkanSwapChain.SwapChainExtent);

	DrawObjects(t_MainBuffer);
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), t_MainBuffer);
	m_VulkanDevice.EndCommand(t_MainBuffer);


	VkSubmitInfo t_SubmitInfo{};
	t_SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore t_WaitSemaphores[] = { frameData.ImageAvailableSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	t_SubmitInfo.waitSemaphoreCount = 1;
	t_SubmitInfo.pWaitSemaphores = t_WaitSemaphores;
	t_SubmitInfo.pWaitDstStageMask = waitStages;

	t_SubmitInfo.commandBufferCount = 1;
	t_SubmitInfo.pCommandBuffers = &t_MainBuffer;

	VkSemaphore t_SignalSemaphores[] = { frameData.RenderFinishedSemaphore };
	t_SubmitInfo.signalSemaphoreCount = 1;
	t_SubmitInfo.pSignalSemaphores = t_SignalSemaphores;



	vkResetFences(m_VulkanDevice, 1, &frameData.InFlightFence);
	if (vkQueueSubmit(mvk_GraphicsQueue, 1, &t_SubmitInfo, frameData.InFlightFence) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR t_PresentInfo{};
	t_PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	t_PresentInfo.waitSemaphoreCount = 1;
	t_PresentInfo.pWaitSemaphores = t_SignalSemaphores;

	VkSwapchainKHR t_SwapChains[] = { m_VulkanSwapChain.SwapChain };
	t_PresentInfo.swapchainCount = 1;
	t_PresentInfo.pSwapchains = t_SwapChains;
	t_PresentInfo.pImageIndices = &t_ImageIndex;

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

void Renderer::DrawObjects(VkCommandBuffer a_CmdBuffer)
{
	//Performance boost when getting the same data needed.
	MeshHandle t_LastMeshData = RENDER_NULL_HANDLE;
	MeshData t_MeshData;

	MaterialHandle t_LastMaterial = RENDER_NULL_HANDLE;
	Material t_Material;
	VkDescriptorSet t_LastMatDescriptorSet = VK_NULL_HANDLE;

	uint32_t t_LastPipelineID = UINT32_MAX;
	PipeLineData t_CurrentPipeLine;

	Material t_CurrentMaterial{};

	//Create this once for performance boost of not creating a mat4 everytime.
	InstanceModel t_PushInstance{};

	for (size_t i = 0; i < m_RenderObjects.size(); i++)
	{
		RenderObject t_RenderObject = m_RenderObjects.at(i);

		if (t_RenderObject.materialHandle != t_LastMaterial)
		{
			t_LastMaterial = t_RenderObject.materialHandle;
			t_Material = m_MaterialPool.Get(t_LastMaterial);

			uint32_t t_PipelineID = t_Material.pipelineID;

			//only bind the pipeline if it doesn't match with the already bound one
			if (t_PipelineID != t_LastPipelineID)
			{
				t_CurrentPipeLine = m_ShaderManager->PipelinePool.Get(t_PipelineID);
				t_LastPipelineID = t_PipelineID;

				vkCmdBindPipeline(a_CmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, t_CurrentPipeLine.pipeLine);

				vkCmdBindDescriptorSets(a_CmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, t_CurrentPipeLine.pipeLineLayout, 0, 1, &m_GlobalSet[m_CurrentFrame], 0, nullptr);
			}
			if (t_LastMatDescriptorSet != t_Material.secondDescriptorSet)
			{
				vkCmdBindDescriptorSets(a_CmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, t_CurrentPipeLine.pipeLineLayout, 1, 1, &t_Material.secondDescriptorSet, 0, nullptr);
			}
		}

		//Set the Push constant data.
		t_PushInstance.model = t_RenderObject.p_Transform->GetModelMatrix();

		vkCmdPushConstants(a_CmdBuffer, t_CurrentPipeLine.pipeLineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(InstanceModel), &t_PushInstance);

		//only bind the mesh if it's a different one from last bind
		if (t_RenderObject.meshHandle != t_LastMeshData) {

			t_MeshData = m_MeshPool.Get(t_RenderObject.meshHandle);

			//bind the mesh vertex buffer with offset 0
			VkDeviceSize t_VertOffset = 0;
			//Set Vertex
			vkCmdBindVertexBuffers(a_CmdBuffer, 0, 1, &t_MeshData.vertices.buffer, &t_VertOffset);

			//Set Indices
			vkCmdBindIndexBuffer(a_CmdBuffer, t_MeshData.indices.buffer, 0, VK_INDEX_TYPE_UINT32);

			
		}

		//vkCmdDraw(r_CmdBuffer, static_cast<uint32_t>(t_RenderObject->GetVertexData()->GetElementCount()), 1, 0, 0);
		vkCmdDrawIndexed(a_CmdBuffer, static_cast<uint32_t>(t_MeshData.indices.elementSize), 1, 0, 0, 0);
	}
}

void Renderer::UpdateUniformBuffer(uint32_t a_CurrentImage)
{
	ViewProjection ubo = p_ActiveCamera->GetViewProjection();

	void* data;
	vkMapMemory(m_VulkanDevice, mvk_ViewProjectionBuffersMemory[a_CurrentImage], 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(m_VulkanDevice, mvk_ViewProjectionBuffersMemory[a_CurrentImage]);
}

bool Renderer::IsDeviceSuitable(VkPhysicalDevice a_Device)
{
	//Queues are supported.
	QueueFamilyIndices t_Indices = FindQueueFamilies(a_Device, *m_Window);

	//Extentions from m_DeviceExtensions are supported.
	bool t_ExtensionsSupported = CheckDeviceExtensionSupport(a_Device);

	//Swapchain is sufficiently supported.
	bool t_SwapChainAdequate = false;
	if (t_ExtensionsSupported) 
	{
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(a_Device, *m_Window);
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