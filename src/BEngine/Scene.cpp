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

    while(auto e = window->mouse.read())
    {
        if(e -> get_type() == Mouse::Event::Type::Move)
        {
            std::cout << "Mouse: (" 
                      << e->get_x() << ", " 
                      << e->get_y() << ")\n";
        }
        else if(e -> get_type() == Mouse::Event::Type::Enter)
            std::cout<<"Mouse Enter\n";
        else if(e -> get_type() == Mouse::Event::Type::Leave)
            std::cout<<"Mouse Leave\n";
    }
    
    Engine::GetRenderer()->RenderFrame();
}


void Scene::Exit()
{

}