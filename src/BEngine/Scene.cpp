#include "Engine.hpp"
#include "Scene.hpp"

#include "Utils/Utils.hpp"

#include "iostream"
Scene::Scene()
{
    window = Engine::GetWindow();
}


void Scene::Run()
{
    if (!window)
        return;
    while (!window->should_close())
        Update();

}


void Scene::Update()
{
    window->poll_events();

    if(window->was_resized())
    {
        Engine::GetRenderer()->UpdateResolution({window->specs.wnd_width, 
                                                 window->specs.wnd_height});
        window->clear_resize_flag();
    }

    while(auto e  = window->keyboard.read_key())
    {
        if(e->is_pressed())
        {
            switch(e->get_code())
            {
                case GLFW_KEY_ESCAPE:
                    window->close();
                    break;
                case GLFW_KEY_P:
                    std::cout<<"WIREFRANE_MODE"<<std::endl;
                    break;
                
                case GLFW_KEY_A:
                    std::cout<<"KEY_A = MOVE_LEFT"<<std::endl;
                    break;
                case GLFW_KEY_D:
                    std::cout<<"KEY_D = MOVE_RIGHT"<<std::endl;
                    break;
                case GLFW_KEY_W:
                    std::cout<<"KEY_W = MOVE_FRONT"<<std::endl;
                    break;
                case GLFW_KEY_S:
                    std::cout<<"KEY_S = MOVE_BACK"<<std::endl;
                    break;
                case GLFW_KEY_Q:
                    std::cout<<"KEY_Q = MOVE_UP"<<std::endl;
                    break;
                case GLFW_KEY_E:
                    std::cout<<"KEY_E = MOVE_DOWN"<<std::endl;
                    break;
                case GLFW_KEY_R:
                    std::cout<<"KEY_R = RANDOM_INT: "<< bengine_rand::rand<uint32_t>(0, 100)<<std::endl; 
                    break;        
                case GLFW_KEY_CAPS_LOCK:
                    std::cout<<"KEY_CAPS_LOCK:"<<std::endl; 
                    break;            
            }   

        }
    }

    // while(auto e = window->mouse.read())
    // {
    //     if(e -> get_type() == Mouse::Event::Type::Move)
    //     {
    //         std::cout << "Mouse: (" 
    //                   << e->get_x() << ", " 
    //                   << e->get_y() << ")\n";
    //     }
    //     else if(e -> get_type() == Mouse::Event::Type::Enter)
    //         std::cout<<"Mouse Enter\n";
    //     else if(e -> get_type() == Mouse::Event::Type::Leave)
    //         std::cout<<"Mouse Leave\n";
    // }
    
    Engine::GetRenderer()->RenderFrame();
}


void Scene::Exit()
{

}