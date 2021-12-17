#pragma once
#include "ObjectManager.h"
#include "CameraController.h"

#include "Scene.h"

#include "Structs/SceneData.h"

class GUISystem;

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
	uint32_t CurrentScene = MAXUINT32;
	std::vector<Scene> m_Scenes;

	Renderer* m_Renderer;
	GUISystem* m_GuiSystem;

	ObjectManager* m_ObjectManager;
	CameraController* m_CameraController;

	RenderFactory* m_RenderFactory;

};

