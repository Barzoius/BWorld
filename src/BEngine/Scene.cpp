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
    window->PoolEvents();
}


void Scene::Exit()
{

}