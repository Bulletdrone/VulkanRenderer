#pragma once

#include <vector>
#include <stdexcept>

#include "SceneData.h"
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


		const char* GetSceneName() { return m_SceneName; }
		SceneData& GetSceneData() { return m_SceneData; }

	private:
		const char* m_SceneName;

		SceneData m_SceneData;
	};
}