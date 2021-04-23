#include "MainRenderer/Renderer.h"

int main() 
{
    Renderer* m_Renderer = new Renderer();

    //while (!glfwWindowShouldClose(m_Renderer->GetWindow())) {
    //    glfwPollEvents();

    //    m_Renderer->RenderUpdate(0);
    //}

    delete m_Renderer;

    return 0;
}