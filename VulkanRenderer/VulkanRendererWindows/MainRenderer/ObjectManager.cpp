#include "ObjectManager.h"
#include "Tools/VulkanInitializers.h"

ObjectManager::ObjectManager(Renderer* a_Renderer)
	: p_Renderer(a_Renderer)
{
	p_Renderer->SetRenderObjectsVector(&m_RenderObjects);

	m_Textures.resize(2);

	//pip_SpaceImage.p_DescriptorData = &m_DescriptorData[0];
	//pip_SpaceImage.p_DescriptorData->texture = &m_Textures[0];
	//Load Texture
	p_Renderer->SetupImage(m_Textures[0], "../Resources/Images/Background.png");
	p_Renderer->SetupImage(m_Textures[1], "../Resources/Images/Background1.png");

	std::vector<TextureData> t_GlobalTextures;
	SetupDescriptor(des_Global, 1, t_GlobalTextures);

	std::vector<TextureData> pav; pav.push_back(m_Textures[0]);
	std::vector<TextureData> tri; tri.push_back(m_Textures[1]);
	SetupDescriptor(des_Pavillion, 0, pav);
	SetupDescriptor(des_Triangle, 0, tri);

	std::vector<uint32_t> t_DescriptorsPav{ des_Global, des_Pavillion };
	//des_Pavillion = p_Renderer->CreateDescriptorLayout(&m_Textures. );
	pip_Pavillion = p_Renderer->CreateGraphicsPipeline(t_DescriptorsPav);

	std::vector<uint32_t> t_DescriptorsTri{ des_Global, des_Triangle };
	pip_Triangle = p_Renderer->CreateGraphicsPipeline(t_DescriptorsTri);
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

	VkDescriptorBufferInfo buffer1 = VkInit::CreateDescriptorBufferInfo(p_Renderer->mvk_ViewProjectionBuffers[0], 0, sizeof(ViewProjection));
	VkDescriptorBufferInfo buffer2 = VkInit::CreateDescriptorBufferInfo(p_Renderer->mvk_ViewProjectionBuffers[1], 0, sizeof(ViewProjection));

	VkDescriptorImageInfo image1 = VkInit::CreateDescriptorImageInfo(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_Textures[0].textureImageView, m_Textures[0].textureSampler);
	VkDescriptorImageInfo image2 = image1;

	VkDescriptorImageInfo image1Tri = VkInit::CreateDescriptorImageInfo(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_Textures[1].textureImageView, m_Textures[1].textureSampler);
	VkDescriptorImageInfo image2Tri = image1Tri;

	std::vector<VkDescriptorBufferInfo> buffers{ buffer1 , buffer2 };
	std::vector<VkDescriptorImageInfo> images{ image1, image2 };
	std::vector<VkDescriptorImageInfo> imagesTri{ image1Tri, image2Tri };

	p_Renderer->CreateDescriptorPool(des_Global);
	p_Renderer->CreateDescriptorSet(des_Global, &buffers, nullptr);

	p_Renderer->CreateDescriptorPool(des_Pavillion);
	p_Renderer->CreateDescriptorSet(des_Pavillion, nullptr, &images);

	p_Renderer->CreateDescriptorPool(des_Triangle);
	p_Renderer->CreateDescriptorSet(des_Triangle, nullptr, &imagesTri);
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

void ObjectManager::SetupDescriptor(uint32_t& a_DesID, uint32_t a_BufferCount, std::vector<TextureData>& a_Textures)
{
	a_DesID = p_Renderer->CreateDescriptorLayout(a_Textures, a_BufferCount, nullptr);
}
