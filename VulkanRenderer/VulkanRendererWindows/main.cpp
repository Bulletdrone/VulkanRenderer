#include "MainRenderer/ObjectManager.h"

int main() 
{
    //init's the renderer.
    Renderer* m_Renderer = new Renderer();

    //Setting up the rest.
    m_Renderer->CreateCommandPool();
    m_Renderer->CreateDepthResources();
    m_Renderer->CreateFrameBuffers();

    ObjectManager* m_ObjectManager = new ObjectManager(m_Renderer);

    Transform* t_Transform1 = new Transform(glm::vec3(0.7f, 0, 0.1), 1);
    Transform* t_Transform2 = new Transform(glm::vec3(-0.7f, 0, 0.2), 1);
    Transform* t_Transform3 = new Transform(glm::vec3(0.0f, -0.7f, 0.3), 1);
    
    Transform* t_Transform4 = new Transform(glm::vec3(0.0f, 0.7f, 0.4), 1);

    m_ObjectManager->CreateShape(ShapeType::Rectangle, t_Transform1);
    m_ObjectManager->CreateShape(ShapeType::Triangle, t_Transform2);
    m_ObjectManager->CreateShape(ShapeType::Triangle, t_Transform3);
    m_ObjectManager->CreateShape(ShapeType::Rectangle, t_Transform4);
    

    m_ObjectManager->SetupStartObjects();

    double t_StartTime = glfwGetTime();
    float deltaTime = 0.1f;
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
        m_ObjectManager->UpdateObjects(deltaTime);

        double t_End = glfwGetTime();
        deltaTime = static_cast<float>(t_End - t_CurrentTime);
    }

    delete m_ObjectManager;
    delete m_Renderer;

    return 0;
}