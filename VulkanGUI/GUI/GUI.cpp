#include "GUI.h"

#pragma warning (push, 0)
#include "ImGUI/imgui.h"


#include "imGUI/Backends/imgui_impl_vulkan.h"
#include "ImGUI/Backends/imgui_impl_glfw.h"
#pragma warning (pop)

namespace GUI
{
	GUISystem::GUISystem(GLFWwindow* a_Window)
		: p_Window(a_Window)
	{}

	GUISystem::~GUISystem()
	{
		for (auto& it : m_GUIWindows)
		{
			static_cast<GUIHandle>(it.first) = GUI_NULL_HANDLE;
			delete it.second;
		}
		m_GUIWindows.clear();

		vkDestroyDescriptorPool(m_Device, m_ImguiPool, nullptr);
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}


	//@param r_CommandBuffer, A commandbuffer that must have begon and must be ended after this function.
	void GUISystem::Init(const VkDevice a_Device, const VkInstance a_Instance, const VkPhysicalDevice a_PhysDevice, const VkQueue a_Queue, const VkRenderPass a_MainRenderPass)
	{
		m_Device = a_Device;

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

		vkCreateDescriptorPool(m_Device, &t_PoolInfo, nullptr, &m_ImguiPool);

		//Setup context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGui_ImplGlfw_InitForVulkan(p_Window, true);

		//setup renderer bindings.
		ImGui_ImplVulkan_InitInfo t_InitInfo = {};
		t_InitInfo.Instance = a_Instance;
		t_InitInfo.PhysicalDevice = a_PhysDevice;
		t_InitInfo.Device = m_Device;
		t_InitInfo.QueueFamily = 0;
		t_InitInfo.Queue = a_Queue;
		t_InitInfo.PipelineCache = VK_NULL_HANDLE;
		t_InitInfo.DescriptorPool = m_ImguiPool;
		t_InitInfo.Allocator = VK_NULL_HANDLE;

		t_InitInfo.MinImageCount = 3;
		t_InitInfo.ImageCount = 3;
		t_InitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

		ImGui_ImplVulkan_Init(&t_InitInfo, a_MainRenderPass);

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
	}

	void GUISystem::Update()
	{
		//imgui new frame
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();

		for (auto& it : m_GUIWindows)
		{
			it.second->Update();
		}

		ImGui::Render();
	}

	GUIHandle GUISystem::CreateGUIWindow(GUICreationData a_CreationData)
	{
		m_HandlerCount++;
		GUIHandle handle = m_HandlerCount;
		m_GUIWindows.emplace(std::make_pair(handle, new GUIWindow(a_CreationData)));
		m_GUIWindows.at(handle)->handle = handle;

		return handle;
	}

	bool GUISystem::DeleteGUIWindow(GUIHandle a_Handle)
	{
		if (a_Handle.IsValid())
		{
			auto it = m_GUIWindows.find(a_Handle);
			if (it != m_GUIWindows.end())
			{
				delete it->second;
				m_GUIWindows.erase(it);
				return true;
			}
			printf("Failed to delete GUIWindow, hash couldn't find an element.");
			return false;
		}
		printf("Failed to delete GUIWindow, handler invalid");
		return false;
	}

	const char* GUISystem::GetGUIWindowName(GUIHandle a_Handle)
	{
		return m_GUIWindows.at(a_Handle)->GetName();
	}
}