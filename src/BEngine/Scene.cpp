#include "Engine.hpp"
#include "Scene.hpp"

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
    window->ProcessInput();
    if(window->keyboard.WasPressed(GLFW_KEY_ESCAPE))
        window->Close();
    window->PoolEvents();
    Engine::GetRenderer()->RenderFrame();
}


void Scene::Exit()
{

}