#include "Scene.h"
namespace Engine
{
	Scene::Scene(const char* a_SceneName, uint32_t a_LoadedObjects[], size_t a_ObjectCount, SceneData a_SceneData)
		: m_SceneName(a_SceneName), m_SceneData(a_SceneData)
	{
		for (size_t i = 0; i < a_ObjectCount; i++)
		{
			m_LoadedModels.push_back(a_LoadedObjects[i]);
		}
	}

	void Scene::AddModelID(uint32_t a_ModelID)
	{
		m_LoadedModels.push_back(a_ModelID);
	}

	bool Scene::RemoveModelID(uint32_t a_ModelID)
	{
		for (size_t i = 0; i < m_LoadedModels.size(); i++)
		{
			if (m_LoadedModels[i] == a_ModelID)
			{
				m_LoadedModels.erase(m_LoadedModels.begin() + i);
				return true;
			}

		}
		throw std::runtime_error("failed to delete ID from m_LoadedModels in a Scene.");
	}
}