#include "Precomp.h"
#include "Scene.h"
namespace Engine
{
	Scene::Scene(const char* a_SceneName, size_t a_ObjectCount, SceneData a_SceneData)
		: m_SceneName(a_SceneName), m_SceneData(a_SceneData)
	{

	}
}