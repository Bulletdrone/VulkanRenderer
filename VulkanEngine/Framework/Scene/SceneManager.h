#pragma once
#include <stdint.h>

#include "Scene.h"

enum class ShapeType;

class Renderer;
class ObjectManager;
class CameraController;
class RenderFactory;
class GUISystem;

struct Transform;
struct Material;

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

		void CreateShape(ShapeType a_ShapeType, Transform* a_Transform, Material& a_Material);

	private:
		uint32_t CurrentScene = -1;
		std::vector<Scene> m_Scenes;

		Renderer* m_Renderer;
		GUISystem* m_GuiSystem;

		ObjectManager* m_ObjectManager;
		CameraController* m_CameraController;

		RenderFactory* m_RenderFactory;
	};
}