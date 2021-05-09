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
	PickPhysicalDevice();
	CreateLogicalDevice();
	
	SetupHandlers();
	CreateSwapChain();
	CreateImageViews();
	CreateRenderPass();

	m_ShaderManager = new ShaderManager(mvk_Device, mvk_SwapChainExtent, mvk_DescriptorSetLayout, mvk_PipelineLayout, mvk_Pipeline);
	m_ShaderManager->CreateDescriptorSetLayout();
	m_ShaderManager->CreateGraphicsPipeline(mvk_RenderPass);

	CreateFrameBuffers();
	CreateCommandPool();
}

Renderer::~Renderer()
{
	vkDeviceWaitIdle(mvk_Device);
	CleanupSwapChain();

	vkDestroyDescriptorSetLayout(mvk_Device, mvk_DescriptorSetLayout, nullptr);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(mvk_Device, mvk_RenderFinishedSemaphore[i], nullptr);
		vkDestroySemaphore(mvk_Device, mvk_ImageAvailableSemaphore[i], nullptr);
		vkDestroyFence(mvk_Device, mvk_InFlightFences[i], nullptr);
	}
	vkDestroyCommandPool(mvk_Device, mvk_CommandPool, nullptr);

	delete m_ShaderManager;
	vkDestroyDevice(mvk_Device, nullptr);

	if (DE_EnableValidationLayers)
		delete DE_VulkanDebug;

	vkDestroySurfaceKHR(mvk_Instance, m_Window->GetSurface(), nullptr);
	vkDestroyInstance(mvk_Instance, nullptr);
	delete m_Window;
}

//Must be done after setting up the physical device.
void Renderer::SetupHandlers()
{
	m_CommandHandler = new CommandHandler(mvk_Device, mvk_CommandPool, mvk_GraphicsQueue);
	m_BufferHandler = new BufferHandler(mvk_Device, mvk_PhysicalDevice, m_CommandHandler);
	
	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(mvk_PhysicalDevice, &properties);
	m_ImageHandler = new ImageHandler(mvk_Device, properties.limits.maxSamplerAnisotropy);
}

//Call this after the creation of Vulkan.
void Renderer::SetupRenderObjects()
{
	m_BufferHandler->CreateUniformBuffers(mvk_ViewProjectionBuffers, mvk_ViewProjectionBuffersMemory, mvk_SwapChainImages.size());
	CreateDescriptorPool();
	CreateDescriptorSets();

	CreateCommandBuffers();
	CreateSyncObjects();
}

void Renderer::CleanupSwapChain()
{
	for (size_t i = 0; i < mvk_SwapChainFrameBuffers.size(); i++) {
		vkDestroyFramebuffer(mvk_Device, mvk_SwapChainFrameBuffers[i], nullptr);
	}

	vkFreeCommandBuffers(mvk_Device, mvk_CommandPool, static_cast<uint32_t>(mvk_CommandBuffers.size()), mvk_CommandBuffers.data());

	vkDestroyPipeline(mvk_Device, mvk_Pipeline, nullptr);
	vkDestroyPipelineLayout(mvk_Device, mvk_PipelineLayout, nullptr);
	vkDestroyRenderPass(mvk_Device, mvk_RenderPass, nullptr);

	for (size_t i = 0; i < mvk_SwapChainImageViews.size(); i++) {
		vkDestroyImageView(mvk_Device, mvk_SwapChainImageViews[i], nullptr);
	}

	vkDestroySwapchainKHR(mvk_Device, mvk_SwapChain, nullptr);

	for (size_t i = 0; i < mvk_SwapChainImages.size(); i++)
	{
		vkDestroyBuffer(mvk_Device, mvk_ViewProjectionBuffers[i], nullptr);
		vkFreeMemory(mvk_Device, mvk_ViewProjectionBuffersMemory[i], nullptr);
	}
	vkDestroyDescriptorPool(mvk_Device, mvk_DescriptorPool, nullptr);
}

void Renderer::RecreateSwapChain()
{
	m_Window->CheckMinimized();

	vkDeviceWaitIdle(mvk_Device);
		
	CleanupSwapChain();

	CreateSwapChain();
	CreateImageViews();
	CreateRenderPass();
	m_ShaderManager->CreateDescriptorSetLayout();
	m_ShaderManager->CreateGraphicsPipeline(mvk_RenderPass);
	CreateFrameBuffers();

	SetupRenderObjects();
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

void Renderer::PickPhysicalDevice()
{
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
				mvk_PhysicalDevice = device;
				break;
			}
		}
	}

	if (mvk_PhysicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

void Renderer::CreateLogicalDevice()
{
	QueueFamilyIndices t_Indices = FindQueueFamilies(mvk_PhysicalDevice);

	std::vector<VkDeviceQueueCreateInfo> t_QueueCreateInfos;
	std::set<uint32_t> t_UniqueQueueFamilies = { t_Indices.graphicsFamily.value(), t_Indices.presentFamily.value() };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : t_UniqueQueueFamilies) 
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		t_QueueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures t_DeviceFeatures{};
	t_DeviceFeatures.samplerAnisotropy = VK_TRUE;

	VkDeviceCreateInfo t_CreateInfo{};
	t_CreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	t_CreateInfo.queueCreateInfoCount = static_cast<uint32_t>(t_QueueCreateInfos.size());
	t_CreateInfo.pQueueCreateInfos = t_QueueCreateInfos.data();

	t_CreateInfo.pEnabledFeatures = &t_DeviceFeatures;

	t_CreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
	t_CreateInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();

	if (DE_EnableValidationLayers) 
	{
		DE_VulkanDebug->AddDebugLayerInstanceDevice(t_CreateInfo);
	}
	else 
	{
		t_CreateInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(mvk_PhysicalDevice, &t_CreateInfo, nullptr, &mvk_Device) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(mvk_Device, t_Indices.graphicsFamily.value(), 0, &mvk_GraphicsQueue);
	vkGetDeviceQueue(mvk_Device, t_Indices.presentFamily.value(), 0, &mvk_PresentQueue);
}

void Renderer::CreateSwapChain()
{
	SwapChainSupportDetails t_SwapChainSupport = QuerySwapChainSupport(mvk_PhysicalDevice);

	//Getting the needed values from the PhysicalDevice.
	VkSurfaceFormatKHR t_SurfaceFormat = ChooseSwapSurfaceFormat(t_SwapChainSupport.formats);
	VkPresentModeKHR t_PresentMode = ChooseSwapPresentMode(t_SwapChainSupport.presentModes);
	VkExtent2D t_Extent = ChooseSwapExtent(t_SwapChainSupport.capabilities);

	//Setting up the SwapChain.
	uint32_t t_ImageCount = t_SwapChainSupport.capabilities.minImageCount + 1;

	if (t_SwapChainSupport.capabilities.maxImageCount > 0 && t_ImageCount > t_SwapChainSupport.capabilities.maxImageCount) 
	{
		t_ImageCount = t_SwapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR t_CreateInfo{};
	t_CreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	t_CreateInfo.surface = m_Window->GetSurface();

	t_CreateInfo.minImageCount = t_ImageCount;
	t_CreateInfo.imageFormat = t_SurfaceFormat.format;
	t_CreateInfo.imageColorSpace = t_SurfaceFormat.colorSpace;
	t_CreateInfo.imageExtent = t_Extent;
	t_CreateInfo.imageArrayLayers = 1;
	t_CreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices t_Indices = FindQueueFamilies(mvk_PhysicalDevice);
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

	if (vkCreateSwapchainKHR(mvk_Device, &t_CreateInfo, nullptr, &mvk_SwapChain) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(mvk_Device, mvk_SwapChain, &t_ImageCount, nullptr);
	mvk_SwapChainImages.resize(t_ImageCount);
	vkGetSwapchainImagesKHR(mvk_Device, mvk_SwapChain, &t_ImageCount, mvk_SwapChainImages.data());

	//Setting the Local variables
	mvk_SwapChainImageFormat = t_SurfaceFormat.format;
	mvk_SwapChainExtent = t_Extent;
}

void Renderer::CreateImageViews()
{
	mvk_SwapChainImageViews.resize(mvk_SwapChainImages.size());

	for (uint32_t i = 0; i < mvk_SwapChainImages.size(); i++) 
	{
		mvk_SwapChainImageViews[i] = m_ImageHandler->CreateImageView(mvk_SwapChainImages[i], mvk_SwapChainImageFormat);
	}
}

void Renderer::CreateRenderPass()
{
	VkAttachmentDescription t_ColorAttachment{};
	t_ColorAttachment.format = mvk_SwapChainImageFormat;
	t_ColorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

	t_ColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	t_ColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

	t_ColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	t_ColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	t_ColorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	t_ColorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	//Subpasses and Attachment
	VkAttachmentReference t_ColorAttachmentRef{};
	t_ColorAttachmentRef.attachment = 0;
	t_ColorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription t_Subpass{};
	t_Subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

	t_Subpass.colorAttachmentCount = 1;
	t_Subpass.pColorAttachments = &t_ColorAttachmentRef;

	//Setting the struct.
	VkRenderPassCreateInfo t_RenderPassInfo{};
	t_RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	t_RenderPassInfo.attachmentCount = 1;
	t_RenderPassInfo.pAttachments = &t_ColorAttachment;
	t_RenderPassInfo.subpassCount = 1;
	t_RenderPassInfo.pSubpasses = &t_Subpass;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;

	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;

	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	t_RenderPassInfo.dependencyCount = 1;
	t_RenderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(mvk_Device, &t_RenderPassInfo, nullptr, &mvk_RenderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}
}

void Renderer::CreateDescriptorPool()
{
	std::array<VkDescriptorPoolSize, 2> t_PoolSizes{};
	t_PoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	t_PoolSizes[0].descriptorCount = static_cast<uint32_t>(mvk_SwapChainImages.size());
	t_PoolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	t_PoolSizes[1].descriptorCount = static_cast<uint32_t>(mvk_SwapChainImages.size());

	VkDescriptorPoolCreateInfo t_PoolInfo{};
	t_PoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	t_PoolInfo.poolSizeCount = static_cast<uint32_t>(t_PoolSizes.size());
	t_PoolInfo.pPoolSizes = t_PoolSizes.data();
	t_PoolInfo.maxSets = static_cast<uint32_t>(mvk_SwapChainImages.size());

	if (vkCreateDescriptorPool(mvk_Device, &t_PoolInfo, nullptr, &mvk_DescriptorPool) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void Renderer::CreateDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> t_Layouts(mvk_SwapChainImages.size(), mvk_DescriptorSetLayout);
	
	VkDescriptorSetAllocateInfo t_AllocInfo{};
	t_AllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	t_AllocInfo.descriptorPool = mvk_DescriptorPool;
	t_AllocInfo.descriptorSetCount = static_cast<uint32_t>(mvk_SwapChainImages.size());
	t_AllocInfo.pSetLayouts = t_Layouts.data();
	
	mvk_DescriptorSets.resize(mvk_SwapChainImages.size());
	if (vkAllocateDescriptorSets(mvk_Device, &t_AllocInfo, mvk_DescriptorSets.data()) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	for (size_t i = 0; i < mvk_SwapChainImages.size(); i++) 
	{
		VkDescriptorBufferInfo t_BufferInfo{};
		t_BufferInfo.buffer = mvk_ViewProjectionBuffers[i];
		t_BufferInfo.offset = 0;
		t_BufferInfo.range = sizeof(ViewProjection);

		VkDescriptorImageInfo t_ImageInfo{};
		t_ImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		t_ImageInfo.imageView = p_Textures->at(0).textureImageView;
		t_ImageInfo.sampler = p_Textures->at(0).textureSampler;

		std::array<VkWriteDescriptorSet, 2> t_DescriptorWrites{};
		//Uniform Info
		t_DescriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		t_DescriptorWrites[0].dstSet = mvk_DescriptorSets[i];
		t_DescriptorWrites[0].dstBinding = 0;
		t_DescriptorWrites[0].dstArrayElement = 0;
		t_DescriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		t_DescriptorWrites[0].descriptorCount = 1;
		t_DescriptorWrites[0].pBufferInfo = &t_BufferInfo;
		//Image Info
		t_DescriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		t_DescriptorWrites[1].dstSet = mvk_DescriptorSets[i];
		t_DescriptorWrites[1].dstBinding = 1;
		t_DescriptorWrites[1].dstArrayElement = 0;
		t_DescriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		t_DescriptorWrites[1].descriptorCount = 1;
		t_DescriptorWrites[1].pImageInfo = &t_ImageInfo;

		vkUpdateDescriptorSets(mvk_Device, static_cast<uint32_t>(t_DescriptorWrites.size()), 
			t_DescriptorWrites.data(), 0, nullptr);
	}
}

void Renderer::CreateFrameBuffers()
{
	mvk_SwapChainFrameBuffers.resize(mvk_SwapChainImageViews.size());

	for (size_t i = 0; i < mvk_SwapChainImageViews.size(); i++)
	{
		VkImageView attachments[] = 
		{
			mvk_SwapChainImageViews[i]
		};

		VkFramebufferCreateInfo t_FramebufferInfo{};
		t_FramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		t_FramebufferInfo.renderPass = mvk_RenderPass;
		t_FramebufferInfo.attachmentCount = 1;
		t_FramebufferInfo.pAttachments = attachments;
		t_FramebufferInfo.width = mvk_SwapChainExtent.width;
		t_FramebufferInfo.height = mvk_SwapChainExtent.height;
		t_FramebufferInfo.layers = 1;

		if (vkCreateFramebuffer(mvk_Device, &t_FramebufferInfo, nullptr, &mvk_SwapChainFrameBuffers[i]) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void Renderer::CreateCommandPool()
{
	QueueFamilyIndices t_QueueFamilyIndices = FindQueueFamilies(mvk_PhysicalDevice);

	VkCommandPoolCreateInfo t_PoolInfo{};
	t_PoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	t_PoolInfo.queueFamilyIndex = t_QueueFamilyIndices.graphicsFamily.value();
	t_PoolInfo.flags = 0; // Optional

	if (vkCreateCommandPool(mvk_Device, &t_PoolInfo, nullptr, &mvk_CommandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
}

void Renderer::CreateCommandBuffers()
{
	mvk_CommandBuffers.resize(mvk_SwapChainFrameBuffers.size());

	VkCommandBufferAllocateInfo t_AllocInfo{};
	t_AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	t_AllocInfo.commandPool = mvk_CommandPool;
	t_AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	t_AllocInfo.commandBufferCount = (uint32_t)mvk_CommandBuffers.size();

	if (vkAllocateCommandBuffers(mvk_Device, &t_AllocInfo, mvk_CommandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}

	for (size_t i = 0; i < mvk_CommandBuffers.size(); i++) {
		VkCommandBufferBeginInfo t_BeginInfo{};
		t_BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		t_BeginInfo.flags = 0; // Optional
		t_BeginInfo.pInheritanceInfo = nullptr; // Optional

		if (vkBeginCommandBuffer(mvk_CommandBuffers[i], &t_BeginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo t_RenderPassInfo{};
		t_RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		t_RenderPassInfo.renderPass = mvk_RenderPass;
		t_RenderPassInfo.framebuffer = mvk_SwapChainFrameBuffers[i];

		t_RenderPassInfo.renderArea.offset = { 0, 0 };
		t_RenderPassInfo.renderArea.extent = mvk_SwapChainExtent;

		VkClearValue t_ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		t_RenderPassInfo.clearValueCount = 1;
		t_RenderPassInfo.pClearValues = &t_ClearColor;

		//Begin the drawing.
		vkCmdBeginRenderPass(mvk_CommandBuffers[i], &t_RenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(mvk_CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, mvk_Pipeline);
		
		//BIND THE UNIFORM BUFFER.
		vkCmdBindDescriptorSets(mvk_CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, mvk_PipelineLayout, 0, 1, &mvk_DescriptorSets[i], 0, nullptr);
		for (size_t j = 0; j < p_RenderObjects->size(); j++)
		{
			VkBuffer t_VertexBuffers[] = { p_RenderObjects->at(j)->GetVertexData()->GetBuffer() };
			VkDeviceSize t_Offsets[] = { 0 };
			vkCmdBindVertexBuffers(mvk_CommandBuffers[i], 0, 1, t_VertexBuffers, t_Offsets);

			vkCmdBindIndexBuffer(mvk_CommandBuffers[i], p_RenderObjects->at(j)->GetIndexData()->GetBuffer(), 0, VK_INDEX_TYPE_UINT16);

			//ConstantPushing, look in ShaderManager for more information.
			vkCmdPushConstants(mvk_CommandBuffers[i], mvk_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(InstanceModel), &p_RenderObjects->at(j)->GetModelMatrix());

			//Draw the test triangles.
			//vkCmdDraw(mvk_CommandBuffers[i], static_cast<uint32_t>(TEMPMESH->GetVertexData()->GetElementCount()), 1, 0, 0);
			vkCmdDrawIndexed(mvk_CommandBuffers[i], static_cast<uint32_t>(p_RenderObjects->at(j)->GetIndexData()->GetElementCount()), 1, 0, 0, 0);
		}

		vkCmdEndRenderPass(mvk_CommandBuffers[i]);

		if (vkEndCommandBuffer(mvk_CommandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}

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
		if (vkCreateSemaphore(mvk_Device, &t_SemaphoreInfo, nullptr, &mvk_ImageAvailableSemaphore[i]) != VK_SUCCESS ||
			vkCreateSemaphore(mvk_Device, &t_SemaphoreInfo, nullptr, &mvk_RenderFinishedSemaphore[i]) != VK_SUCCESS ||
			vkCreateFence(mvk_Device, &t_FenceInfo, nullptr, &mvk_InFlightFences[i]) != VK_SUCCESS)  
		{
			throw std::runtime_error("failed to create semaphores for a frame!");
		}
	}
}

void Renderer::SetupMesh(MeshData* a_MeshData)
{
	m_BufferHandler->CreateVertexBuffers(a_MeshData->GetVertexData());
	m_BufferHandler->CreateIndexBuffers(a_MeshData->GetIndexData());
}

void Renderer::SetupImage(TextureData& a_TextureData, const char* a_ImagePath)
{
	VkDeviceSize t_Size;
	int t_Width, t_Height, t_Channels;

	m_BufferHandler->CreateTextureImage(a_TextureData.textureImage, a_TextureData.textureImageMemory,
		a_ImagePath, t_Size, t_Width, t_Height, t_Channels);

	a_TextureData.textureImageView = m_ImageHandler->CreateImageView(a_TextureData.textureImage, VK_FORMAT_R8G8B8A8_SRGB);
	a_TextureData.textureSampler = m_ImageHandler->CreateTextureSampler();
}

void Renderer::DrawFrame(uint32_t& r_ImageIndex, float a_dt)
{
	bool recreateSwapChain = false;

	vkWaitForFences(mvk_Device, 1, &mvk_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

	VkResult t_Result = vkAcquireNextImageKHR(mvk_Device, mvk_SwapChain, UINT64_MAX, mvk_ImageAvailableSemaphore[m_CurrentFrame], VK_NULL_HANDLE, &r_ImageIndex);

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
		vkWaitForFences(mvk_Device, 1, &mvk_ImagesInFlight[r_ImageIndex], VK_TRUE, UINT64_MAX);
	}
	// Mark the image as now being in use by this frame
	mvk_ImagesInFlight[r_ImageIndex] = mvk_InFlightFences[m_CurrentFrame];

	VkSubmitInfo t_SubmitInfo{};
	t_SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore t_WaitSemaphores[] = { mvk_ImageAvailableSemaphore[m_CurrentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	t_SubmitInfo.waitSemaphoreCount = 1;
	t_SubmitInfo.pWaitSemaphores = t_WaitSemaphores;
	t_SubmitInfo.pWaitDstStageMask = waitStages;

	t_SubmitInfo.commandBufferCount = 1;
	t_SubmitInfo.pCommandBuffers = &mvk_CommandBuffers[r_ImageIndex];

	VkSemaphore t_SignalSemaphores[] = { mvk_RenderFinishedSemaphore[m_CurrentFrame] };
	t_SubmitInfo.signalSemaphoreCount = 1;
	t_SubmitInfo.pSignalSemaphores = t_SignalSemaphores;


	vkResetFences(mvk_Device, 1, &mvk_InFlightFences[m_CurrentFrame]);
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

	m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::UpdateUniformBuffer(uint32_t a_CurrentImage, float a_dt)
{
	ViewProjection ubo{};
	//ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), mvk_SwapChainExtent.width / (float)mvk_SwapChainExtent.height, 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;

	void* data;
	vkMapMemory(mvk_Device, mvk_ViewProjectionBuffersMemory[a_CurrentImage], 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(mvk_Device, mvk_ViewProjectionBuffersMemory[a_CurrentImage]);
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