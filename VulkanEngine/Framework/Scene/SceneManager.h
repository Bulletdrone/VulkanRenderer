#pragma once
#include <stdint.h>

#include "Scene.h"
#include "GUI/GUI.h"

class Renderer;
class CameraController;
class RenderFactory;
namespace GUI { class GUISystem; }

class Transform;
class Material;

struct SceneObjectCreationGUI
{
	GUI::GUITypes::ITextSlider* MaterialSlider;
	GUI::GUITypes::GetPathButton* PathButton;

	std::vector<MaterialHandle> Materials;

	void AddMaterial(MaterialHandle a_Rh, const char* a_Materialname);
};

namespace Engine 
{
	class SceneManager
	{
	public:
		SceneManager();
		~SceneManager();

		void UpdateScene(float a_Dt);

		void NewScene(const char* a_SceneName, SceneData a_SceneData, bool a_SetScene);
		void SetScene(uint32_t a_SceneID);

		void CreateRenderObject();

	private:
		int32_t CurrentScene = -1;
		std::vector<Scene> m_Scenes;

		Renderer* m_Renderer;
		GUI::GUISystem* m_GuiSystem;
		GUIHandle m_MainWindowHandle;

		CameraController* m_CameraController;

		SceneObjectCreationGUI m_CreationWindow;
	};
}