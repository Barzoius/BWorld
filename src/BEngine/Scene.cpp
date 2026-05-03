#include "Engine.hpp"
#include "Scene.hpp"

#include "iostream"
Scene::Scene()
{
    window = Engine::GetWindow();
}


void Scene::Run()
{
    if (!window)
        return;
    while (!window->ShouldClose())
        Update();

}


void Scene::Update()
{
    window->PoolEvents();

    while(auto e  = window->keyboard.ReadKey())
    {
        if(e->IsPressed())
        {
            if (e->GetCode() == GLFW_KEY_ESCAPE)
            {
                window->Close();
            }
        }
    }
    
    Engine::GetRenderer()->RenderFrame();
}


void Scene::Exit()
{

}