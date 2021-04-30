#include "MainRenderer/Renderer.h"
#include "MainRenderer/ObjectManager.h"

int main() 
{
    //init's the renderer.
    Renderer* m_Renderer = new Renderer();

    ObjectManager* m_ObjectManager = new ObjectManager(m_Renderer);

    while (!glfwWindowShouldClose(m_Renderer->GetWindow())) 
    {
        glfwPollEvents();
        m_ObjectManager->UpdateObjects(0);
    }


    delete m_Renderer;

    return 0;
}