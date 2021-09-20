#include "ObjectManager.h"
#include "Tools/VulkanInitializers.h"

ObjectManager::ObjectManager(Renderer* a_Renderer)
	: p_Renderer(a_Renderer)
{
	m_DescriptorAllocator = new DescriptorAllocator();
	m_DescriptorAllocator->Init(p_Renderer->GetVulkanDevice());

	m_DescriptorLayoutCache = new DescriptorLayoutCache();
	m_DescriptorLayoutCache->Init(p_Renderer->GetVulkanDevice());


	VkDescriptorSetLayoutBinding t_CameraBinding = VkInit::CreateDescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0);

	VkDescriptorSetLayoutCreateInfo t_CameraLayoutInfo{};
	t_CameraLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	t_CameraLayoutInfo.bindingCount = 1;
	t_CameraLayoutInfo.pBindings = &t_CameraBinding;

	layout_SingleBufferCamera = m_DescriptorLayoutCache->CreateLayout(&t_CameraLayoutInfo);

	VkDescriptorSetLayoutBinding t_ImageBinding = VkInit::CreateDescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1);

	VkDescriptorSetLayoutCreateInfo t_ImageLayoutInfo{};
	t_ImageLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	t_ImageLayoutInfo.bindingCount = 1;
	t_ImageLayoutInfo.pBindings = &t_ImageBinding;

	layout_SingleImageObject = m_DescriptorLayoutCache->CreateLayout(&t_ImageLayoutInfo);


	p_Renderer->SetRenderObjectsVector(&m_RenderObjects);

	m_Textures.resize(2);

	//pip_SpaceImage.p_DescriptorData = &m_DescriptorData[0];
	//pip_SpaceImage.p_DescriptorData->texture = &m_Textures[0];
	//Load Texture
	p_Renderer->SetupImage(m_Textures[0], "../Resources/Images/Background.png");
	p_Renderer->SetupImage(m_Textures[1], "../Resources/Images/Background1.png");

	//std::vector<TextureData> t_GlobalTextures;
	//SetupDescriptor(des_Global, 1, t_GlobalTextures);

	//std::vector<TextureData> pav; pav.push_back(m_Textures[0]);
	//std::vector<TextureData> tri; tri.push_back(m_Textures[1]);
	//SetupDescriptor(des_Pavillion, 0, pav);
	//SetupDescriptor(des_Triangle, 0, tri);

	//des_Pavillion = p_Renderer->CreateDescriptorLayout(&m_Textures. );
	std::vector<VkDescriptorSetLayout> r_DescriptorLayouts{ layout_SingleBufferCamera, layout_SingleImageObject };
	
	mat_Pavillion.PipelineID = p_Renderer->CreateGraphicsPipeline(r_DescriptorLayouts);
	mat_Rectangle.PipelineID = p_Renderer->CreateGraphicsPipeline(r_DescriptorLayouts);
}

ObjectManager::~ObjectManager()
{
	for (size_t i = 0; i < m_RenderObjects.size(); i++)
	{
		//m_RenderObjects[i]->GetMeshData()->DeleteBuffers(p_Renderer->GetVulkanDevice().m_LogicalDevice);
		delete m_RenderObjects[i];
	}
	m_RenderObjects.clear();
}

void ObjectManager::SetupStartObjects()
{
	p_Renderer->SetupRenderObjects();

	DescriptorBuilder t_Builder;

	VkDescriptorBufferInfo buffer1 = VkInit::CreateDescriptorBufferInfo(p_Renderer->mvk_ViewProjectionBuffers[0], 0, sizeof(ViewProjection));
	t_Builder = DescriptorBuilder::Begin(m_DescriptorLayoutCache, m_DescriptorAllocator);
	t_Builder.BindBuffer(0, &buffer1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	t_Builder.Build(p_Renderer->GlobalSet[0], layout_SingleBufferCamera);

	VkDescriptorBufferInfo buffer2 = VkInit::CreateDescriptorBufferInfo(p_Renderer->mvk_ViewProjectionBuffers[1], 0, sizeof(ViewProjection));
	t_Builder = DescriptorBuilder::Begin(m_DescriptorLayoutCache, m_DescriptorAllocator);
	t_Builder.BindBuffer(0, &buffer2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	t_Builder.Build(p_Renderer->GlobalSet[1], layout_SingleBufferCamera);

	VkDescriptorImageInfo image = VkInit::CreateDescriptorImageInfo(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_Textures[0].textureImageView, m_Textures[0].textureSampler);
	t_Builder = DescriptorBuilder::Begin(m_DescriptorLayoutCache, m_DescriptorAllocator);
	t_Builder.BindImage(1, &image, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	t_Builder.Build(mat_Pavillion.GetDescriptorSet(), layout_SingleImageObject);

	VkDescriptorImageInfo imageTri = VkInit::CreateDescriptorImageInfo(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_Textures[1].textureImageView, m_Textures[1].textureSampler);
	t_Builder = DescriptorBuilder::Begin(m_DescriptorLayoutCache, m_DescriptorAllocator);
	t_Builder.BindImage(1, &imageTri, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	t_Builder.Build(mat_Rectangle.GetDescriptorSet(), layout_SingleImageObject);
}

void ObjectManager::UpdateObjects(float a_Dt)
{
	uint32_t t_ImageIndex;

	for (size_t i = 0; i < m_RenderObjects.size(); i++)
	{
		m_RenderObjects[i]->Update();
	}

	p_Renderer->DrawFrame(t_ImageIndex, a_Dt);

	//Update Uniform Buffer
	//p_Renderer->UpdateUniformBuffer(t_ImageIndex, a_Dt);
}

void ObjectManager::AddRenderObject(BaseRenderObject* a_NewShape)
{
	m_RenderObjects.push_back(a_NewShape);
}
