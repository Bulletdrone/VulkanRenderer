#pragma once

#include <vector>
#include <stdexcept>

#include "Structs/SceneData.h"

class Scene
{
public:
	/*  Create a Scene with a list of Models it must load.
	@param a_SceneName, The name of the Scene.
	@param a_LoadedObjects, Array of uint's that are used to pre-load objects using the RenderFactory in SceneManager.
	@param a_ObjectCount, The size of the a_LoadedObjects array.
	@param a_SceneData, The SceneData specific for this Scene, does the fog and main directional light. */
	Scene(const char* a_SceneName, uint32_t a_LoadedObjects[], size_t a_ObjectCount, SceneData a_SceneData);
	~Scene() {};

	/*  Create a Scene with a list of Models it must load.
	@param a_ModelID, The ID of the Model that is loaded in the Renderer. */
	void AddModelID(uint32_t a_ModelID);

	/*  Get a filled object on a array position.
	@param a_ModelID, The ID of the model you want to remove from the Scene.
	@return returns true if the ID was deleted, returns false if the ID wasn't found. This likely means an error in the program. */
	bool RemoveModelID(uint32_t a_ModelID);


	const char* GetSceneName() { return m_SceneName; }
	const std::vector<uint32_t>& GetLoadedModels() { return m_LoadedModels; }
	size_t GetLoadedModelCount() { return m_LoadedModels.size(); }
	SceneData& GetSceneData() { return m_SceneData; }

private:
	const char* m_SceneName;
	std::vector<uint32_t> m_LoadedModels;

	SceneData m_SceneData;
};