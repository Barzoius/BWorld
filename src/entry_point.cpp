#include "Renderer.hpp"
#include "GLFW/glfw3.h"
#include <iostream>
#include "glm/common.hpp"
#include "glm/vec3.hpp"

#include "Engine.hpp"
#include "Scene.hpp"


int main()
{
    WND_SPECS specs;
    specs.wnd_width = 1280;
    specs.wnd_height = 720;
    specs.wnd_name = "WORLD_1";
    
    (void)Engine::Init(specs);

    Scene* scene = new Scene();


    scene -> Run();
    scene -> Update();
    //scene -> Exit();

    return 0;
}