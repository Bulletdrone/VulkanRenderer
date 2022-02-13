#include "Precomp.h"
#include "Scene.h"
namespace Engine
{
	Scene::Scene(const char* a_SceneName, SceneData a_SceneData)
		: m_SceneName(a_SceneName), m_SceneData(a_SceneData)
	{

	}
	SceneObject& Scene::AddSceneObject(SceneObject a_SceneObject)
	{
		m_SceneObjects.push_back(a_SceneObject);
		return m_SceneObjects.back();
	}
}