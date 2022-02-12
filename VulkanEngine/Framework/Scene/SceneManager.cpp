#include "Precomp.h"
#include "SceneManager.h"


#include "CameraController.h"

#include "ResourceSystem/ResourceAllocator.h"
#include "ResourceSystem/TextureResource.h"
#include "ResourceSystem/MeshResource.h"

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


		m_ObjectManager = new ObjectManager(m_Renderer);
		m_CameraController = new CameraController(m_Renderer);

		//Setup GUI
		m_GuiSystem = new GUI::GUISystem(m_Renderer->GetWindow());
		m_Renderer->SetupGUIsystem(m_GuiSystem);

		GUI::GUICreationData mainWindow{};
		mainWindow.position = glm::vec2(0, 0);
		mainWindow.scale = glm::vec2(300, 100);
		mainWindow.windowName = "Test Window";

		GUIHandle windowHandle = m_GuiSystem->CreateGUIWindow(mainWindow);

		GUI::GUITypes::StaticText t_Text{};
		t_Text.value = "Vulkan Renderer says hello to the user.";
		m_GuiSystem->AddElementToGUIWindow(windowHandle, t_Text);

		//Creation Window
		GUI::GUITypes::GetPathButton t_PathButton{};
		t_PathButton.name = "Load Mesh";
		t_PathButton.fileFilter = DIR_PATH::OBJFILE;
		m_CreationWindow.PathButton = m_GuiSystem->AddElementToGUIWindow(windowHandle, t_PathButton);

		GUI::GUITypes::ITextSlider t_TextSlider{};
		t_TextSlider.name = "Materials";
		m_CreationWindow.MaterialSlider = m_GuiSystem->AddElementToGUIWindow(windowHandle, t_TextSlider);

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

			if (m_CreationWindow.PathButton->active)
			{
				m_CreationWindow.CreateRenderObject(m_ObjectManager);
			}

			double t_End = glfwGetTime();
			deltaTime = static_cast<float>(t_End - t_CurrentTime);
		}

		m_Renderer->CleanupSwapChain();
	}

	void SceneManager::NewScene(const char* a_SceneName, SceneData a_SceneData, bool a_SetScene)
	{
		Scene scene(a_SceneName, a_SceneData);

		m_Scenes.push_back(scene);

		if (a_SetScene)
			SetScene(static_cast<uint32_t>(m_Scenes.size()) - 1);
	}

	void SceneManager::SetScene(uint32_t a_SceneID)
	{
		CurrentScene = static_cast<uint32_t>(a_SceneID);

		Scene t_Scene = m_Scenes[CurrentScene];


		m_Renderer->CreateStartBuffers();

		//Transform* t_PTrans = new Transform(glm::vec3(0, 0, 0), 1);
		m_CreationWindow.AddMaterial(m_Renderer->CreateMaterial(0, nullptr, 1,
			&ResourceAllocator::GetInstance().GetResource<Resource::TextureResource>("../VulkanRenderer/Resources/Images/Background.png", Resource::ResourceType::Texture).texture),
			"Pavillion Material");

		//m_ObjectManager->CreateRenderObject(t_PTrans, mat1,
		//	ResourceAllocator::GetInstance().GetResource<Resource::MeshResource>("../VulkanRenderer/Resources/Models/Pavillion.obj", Engine::Resource::ResourceType::Mesh).meshHandle);

		//Transform* t_Transform2 = new Transform(glm::vec3(-0.7f, 0.9f, 0.2), 1);
		m_CreationWindow.AddMaterial(m_Renderer->CreateMaterial(0, nullptr, 1,
			&ResourceAllocator::GetInstance().GetResource<Resource::TextureResource>("../VulkanRenderer/Resources/Images/Background1.png", Resource::ResourceType::Texture).texture),
			"Space Texture");

		//m_ObjectManager->CreateRenderObject(t_Transform2, mat2, GeometryType::Quad);
	}
}

void SceneObjectCreationGUI::AddMaterial(MaterialHandle a_Rh, const char* a_Materialname)
{
	Materials.push_back(a_Rh);
	MaterialSlider->texts.push_back(a_Materialname);
}

void SceneObjectCreationGUI::CreateRenderObject(ObjectManager* a_ObjectManager)
{
	Transform* t_PTrans = new Transform(glm::vec3(0, 0, 0), 1);

	a_ObjectManager->CreateRenderObject(t_PTrans, Materials[MaterialSlider->currentValue],
		Engine::ResourceAllocator::GetInstance().GetResource<Engine::Resource::MeshResource>(PathButton->filePath, Engine::Resource::ResourceType::Mesh).meshHandle);

}
