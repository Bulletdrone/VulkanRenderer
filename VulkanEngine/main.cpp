#include "Framework/Scene/SceneManager.h"

int main() 
{
    Engine::SceneManager m_SceneManager;

    Engine::SceneData sceneData{};
    sceneData.sunlightDirection = glm::vec4(0, 0, 0, 0);
    sceneData.sunlightColor = glm::vec4(0, 0, 0, 0);

    sceneData.fogDistance = glm::vec4(0, 0, 0, 0);
    sceneData.fogColor = glm::vec4(0, 0, 0, 0);

    sceneData.ambientColor = glm::vec4(0, 0, 0, 0);

    m_SceneManager.NewScene("MainScene", sceneData, true);


    //RendererLoop.
    m_SceneManager.UpdateScene(0);

    return 0;
}