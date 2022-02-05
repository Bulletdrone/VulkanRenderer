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
		@param a_LoadedObjects, Array of uint's that are used to pre-load objects using the RenderFactory in SceneManager.
		@param a_ObjectCount, The size of the a_LoadedObjects array.
		@param a_SceneData, The SceneData specific for this Scene, does the fog and main directional light. */
		Scene(const char* a_SceneName, size_t a_ObjectCount, SceneData a_SceneData);
		~Scene() {};


		const char* GetSceneName() { return m_SceneName; }
		SceneData& GetSceneData() { return m_SceneData; }

	private:
		const char* m_SceneName;

		SceneData m_SceneData;
	};
}