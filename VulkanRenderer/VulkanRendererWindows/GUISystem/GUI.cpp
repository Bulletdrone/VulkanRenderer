#include "GUI.h"

#include "VulkanSwapChain.h"

#pragma warning (push, 0)
#include "ImGUI/imgui.h"

#include "imGUI/imgui_impl_vulkan.h"
#include "ImGUI/imgui_impl_glfw.h"
#pragma warning (pop)


	GUISystem::GUISystem(GLFWwindow* a_Window, VulkanDevice& r_Device, const VulkanSwapChain& r_SwapChain)
		:	rm_Device(r_Device), rm_SwapChain(r_SwapChain), p_Window(a_Window)
	{}

	GUISystem::~GUISystem()
	{
		vkDestroyDescriptorPool(rm_Device, m_ImguiPool, nullptr);
		ImGui_ImplVulkan_Shutdown();
	}

	void GUISystem::Init(VkInstance& r_Instance, VkQueue& r_Queue, VkRenderPass& r_MainRenderPass)
	{
		VkDescriptorPoolSize t_PoolSizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};

		VkDescriptorPoolCreateInfo t_PoolInfo = {};
		t_PoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		t_PoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		t_PoolInfo.maxSets = 1000;
		t_PoolInfo.poolSizeCount = sizeof(t_PoolSizes) / sizeof(t_PoolSizes[0]);
		t_PoolInfo.pPoolSizes = t_PoolSizes;

		vkCreateDescriptorPool(rm_Device, &t_PoolInfo, nullptr, &m_ImguiPool);

		//Setup context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();

		ImGui_ImplGlfw_InitForVulkan(p_Window, true);

		//setup renderer bindings.
		ImGui_ImplVulkan_InitInfo t_InitInfo = {};
		t_InitInfo.Instance = r_Instance;
		t_InitInfo.PhysicalDevice = rm_Device.m_PhysicalDevice;
		t_InitInfo.Device = rm_Device;
		t_InitInfo.QueueFamily = VK_NULL_HANDLE;
		t_InitInfo.Queue = r_Queue;
		t_InitInfo.PipelineCache = VK_NULL_HANDLE;
		t_InitInfo.DescriptorPool = m_ImguiPool;
		t_InitInfo.Allocator = VK_NULL_HANDLE;

		t_InitInfo.MinImageCount = 3;
		t_InitInfo.ImageCount = 3;
		t_InitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

		ImGui_ImplVulkan_Init(&t_InitInfo, r_MainRenderPass);

		VkCommandBuffer t_Cmd = rm_Device.BeginSingleTimeCommands();
		ImGui_ImplVulkan_CreateFontsTexture(t_Cmd);
		rm_Device.EndSingleTimeCommands(t_Cmd);

		ImGui_ImplVulkan_DestroyFontUploadObjects();
		
		//Setup standard font.
		

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
	}

	void GUISystem::Update()
	{
		//imgui new frame
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();

		for (size_t i = 0; i < m_GUIWindows.size(); i++)
		{
			m_GUIWindows[i].Update(false);
		}

		ImGui::Render();

		//imgui commands
		//ImGui::ShowDemoWindow();
	}

	GUIWindow* GUISystem::CreateGUIWindow()
	{
		m_GUIWindows.push_back(GUIWindow());
		return &m_GUIWindows.back();
	}

#pragma region WindowData


	GUIWindow::GUIWindow()
	{}

	GUIWindow::~GUIWindow()
	{
		for (GUITypes::GUIElement* it : m_GUIElements)
		{
			delete it;
		}
		m_GUIElements.clear();
	}

	void GUIWindow::Init(glm::vec2 a_Position, glm::vec2 a_Scale, const char* a_WindowName, bool a_Resizable)
	{
		m_Position = a_Position;
		m_Scale = a_Scale;

		m_WindowName = a_WindowName;
		if (!a_Resizable)
			m_WindowFlags = ImGuiWindowFlags_NoResize | 
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoCollapse;
	}

	bool GUIWindow::Update(bool a_IsChild)
	{
		//if the window is inactive just return.
		if (!m_Active) return false;


		bool t_ValueChanged = false;

		if (!a_IsChild)
		{
			ImGui::SetNextWindowPos(ImVec2(m_Position.x, m_Position.y), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImVec2(m_Scale.x, m_Scale.y), ImGuiCond_Once);

			ImGui::Begin(m_WindowName, NULL, m_WindowFlags);
		}
		else
			ImGui::BeginChild(m_WindowName);

		for (GUITypes::GUIElement* it : m_GUIElements)
		{
			it->DrawElement();
		}

		for (GUIWindow& it : m_ChildWindows)
		{
			it.Update(true);
		}


		if (!a_IsChild)
			ImGui::End();
		else
			ImGui::EndChild();

		return t_ValueChanged;
	}

	void GUIWindow::AddText(std::string* a_Text)
	{
		GUITypes::Text* t_Text = new GUITypes::Text;
		t_Text->value = a_Text;

		m_GUIElements.push_back(t_Text);
	}

	void GUIWindow::AddText(const char* a_Text)
	{
		GUITypes::StaticText* t_Text = new GUITypes::StaticText;
		t_Text->value = a_Text;

		m_GUIElements.push_back(t_Text);
	}

	void GUIWindow::AddColorEdit(const char* a_Name, float* a_Values)
	{
		GUITypes::ColorEdit* t_ColorEdit = new GUITypes::ColorEdit;
		t_ColorEdit->name = a_Name;
		t_ColorEdit->value = a_Values;

		m_GUIElements.push_back(t_ColorEdit);
	}

	void GUIWindow::AddIntSlider(const char* a_Name, int* a_Values, int a_ElementSize, int a_Min, int a_Max)
	{
		GUITypes::ISlider* t_Slider = new GUITypes::ISlider;
		t_Slider->name = a_Name;
		t_Slider->value = a_Values;
		t_Slider->elementSize = a_ElementSize;
		t_Slider->max = a_Max;
		t_Slider->min = a_Min;

		m_GUIElements.push_back(t_Slider);
	}

	void GUIWindow::AddFloatSlider(const char* a_Name, float* a_Values, int a_ElementSize, float a_Min, float a_Max)
	{
		GUITypes::FSlider* t_Slider = new GUITypes::FSlider;
		t_Slider->name = a_Name;
		t_Slider->value = a_Values;
		t_Slider->elementSize = a_ElementSize;
		t_Slider->max = a_Max;
		t_Slider->min = a_Min;

		m_GUIElements.push_back(t_Slider);
	}

	void GUIWindow::AddButton(const char* a_Name, bool* a_Values)
	{
		GUITypes::Button* t_Slider = new GUITypes::Button;
		t_Slider->name = a_Name;
		t_Slider->value = a_Values;

		m_GUIElements.push_back(t_Slider);
	}

	GUIWindow& GUIWindow::CreateChildWindow()
	{
		m_ChildWindows.resize(m_ChildWindows.size() + 1);
		return m_ChildWindows.back();
	}


#pragma endregion