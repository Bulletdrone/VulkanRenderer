#pragma once

#include <vector>
#include <stdexcept>

#include "SceneData.h"
#include "GameComponents/SceneObject.h"

namespace Engine
{
	class Scene
	{
	public:
		/*  Create a Scene with a list of Models it must load.
		@param a_SceneName, The name of the Scene.
		@param a_SceneData, The SceneData specific for this Scene, does the fog and main directional light. */
		Scene(const char* a_SceneName, SceneData a_SceneData);
		~Scene() {};

		SceneObject& AddSceneObject(SceneObject a_SceneObject);
		const std::vector<SceneObject*>& GetSceneObjectList() { return m_SceneObjects; }

		const char* GetSceneName() { return m_SceneName; }
		SceneData& GetSceneData() { return m_SceneData; }

	private:
		std::vector<SceneObject*> m_SceneObjects;

		const char* m_SceneName;

		SceneData m_SceneData;
	};
}