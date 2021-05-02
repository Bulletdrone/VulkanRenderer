#include "MainRenderer/Renderer.h"
#include "MainRenderer/ObjectManager.h"

int main() 
{
    //init's the renderer.
    Renderer* m_Renderer = new Renderer();

    ObjectManager* m_ObjectManager = new ObjectManager(m_Renderer);
    m_ObjectManager->SetupRenderObjects();

    m_Renderer->SetupRenderObjects();


    double t_StartTime = glfwGetTime();
    int t_FrameCount = 0;

    while (!glfwWindowShouldClose(m_Renderer->GetWindow())) 
    {
        double t_CurrentTime = glfwGetTime();

        t_FrameCount++;
        if (t_CurrentTime - t_StartTime >= 1.0)
        {
            printf("%d \n", t_FrameCount);

            t_FrameCount = 0;
            t_StartTime = t_CurrentTime;
        }

        glfwPollEvents();
        m_ObjectManager->UpdateObjects(0);
    }

    delete m_Renderer;

    return 0;
}