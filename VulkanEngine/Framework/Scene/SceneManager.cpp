#include "Precomp.h"
#include "SceneManager.h"

#include "ObjectManager.h"
#include "CameraController.h"
#include "GUI.h"

#include "ResourceSystem/ResourceAllocator.h"
#include "ResourceSystem/TextureResource.h"

namespace Engine
{
	SceneManager::SceneManager()
	{
		m_Renderer = new Renderer();
		ResourceAllocator::GetInstance().SetRenderer(m_Renderer);
		//Setting up the rest.
		m_Renderer->CreateCommandPool();
		m_Renderer->CreateDepthResources();
		m_Renderer->CreateFrameBuffers();

		//m_RenderFactory = new RenderFactory();
		m_ObjectManager = new ObjectManager(m_Renderer);
		m_CameraController = new CameraController(m_Renderer);

		m_GuiSystem = new GUISystem(m_Renderer->GetWindow(), m_Renderer->GetVulkanDevice(), m_Renderer->GetVulkanSwapChain());
		m_GuiSystem->Init(m_Renderer->GetInstance(), m_Renderer->GetQueue(), m_Renderer->GetRenderPass());

		GUIWindow* gUIwindow = m_GuiSystem->CreateGUIWindow();
		gUIwindow->Init(glm::vec2(0, 0), glm::vec2(300, 100), "Test Window", true);

		gUIwindow->AddText("Vulkan Renderer says hello to the user.");

	}

	SceneManager::~SceneManager()
	{
		delete m_CameraController;
		delete m_ObjectManager;

		delete m_Renderer;
	}

	void SceneManager::UpdateScene(float a_Dt)
	{
		(void)a_Dt;
		double t_StartTime = glfwGetTime();
		float deltaTime = 0.1f;
		int t_FrameCount = 0;

		while (!glfwWindowShouldClose(m_Renderer->GetWindow()))
		{
			double t_CurrentTime = glfwGetTime();

			t_FrameCount++;
			if (t_CurrentTime - t_StartTime >= 1.0)
			{
				printf("%d \n", t_FrameCount);

				t_FrameCount = 0;
				t_StartTime = t_CurrentTime;

				//m_CameraController->SwitchNextCamera();
			}

			glfwPollEvents();
			m_CameraController->UpdateActiveCamera();

			m_GuiSystem->Update();
			m_ObjectManager->UpdateObjects(deltaTime);


			double t_End = glfwGetTime();
			deltaTime = static_cast<float>(t_End - t_CurrentTime);
		}

		m_Renderer->CleanupSwapChain();
	}

	void SceneManager::NewScene(const char* a_SceneName, SceneData a_SceneData, bool a_SetScene)
	{
		Scene scene(a_SceneName, 3, a_SceneData);

		m_Scenes.push_back(scene);

		if (a_SetScene)
			SetScene(static_cast<uint32_t>(m_Scenes.size()) - 1);
	}

	void SceneManager::SetScene(uint32_t a_SceneID)
	{
		CurrentScene = static_cast<uint32_t>(a_SceneID);

		Scene t_Scene = m_Scenes[CurrentScene];
		//m_RenderFactory->ResetRenderObjects(t_Scene.GetLoadedModels());

		//Set Uniform Buffer for the SceneData.
		//m_Renderer->SetupImage(ResourceAllocator::GetInstance().GetResource<Resource::TextureResource>("../VulkanRenderer/Resources/Images/Background.png", Resource::ResourceType::Texture).texture);
		//m_Renderer->SetupImage(ResourceAllocator::GetInstance().GetResource<Resource::TextureResource>("../VulkanRenderer/Resources/Images/Background1.png", Resource::ResourceType::Texture).texture);

		Transform* t_PTrans = new Transform(glm::vec3(0, 0, 0), 1);
		m_ObjectManager->CreateRenderObject(t_PTrans, m_ObjectManager->mat_Pavillion, "../VulkanRenderer/Resources/Models/Pavillion.obj");

		Transform* t_Transform2 = new Transform(glm::vec3(-0.7f, 0.9f, 0.2), 1);
		m_ObjectManager->CreateRenderObject(t_Transform2, m_ObjectManager->mat_Rectangle, GeometryType::Quad);

		m_ObjectManager->SetupStartObjects();
	}
}