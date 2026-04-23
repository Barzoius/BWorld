#include <iostream>
#include "glm/common.hpp"
#include "glm/vec3.hpp"

#include "Engine.hpp"
#include "Scene.hpp"

#include "VkRenderer.hpp"

#include "VkContext.hpp"


int main()
{
    WND_SPECS specs;
    specs.wnd_width = 1280;
    specs.wnd_height = 720;
    specs.wnd_name = "WORLD_1";
    
    (void)Engine::Init(specs);

    
    VkContext context;
    context.Initialize(Engine::GetRequiredVulkanExtensions(), 
                                      Engine::GetSurfaceInfo(),
                                      Engine::GetResolution());
    VkRenderer vkR(context);

    Engine::GetRenderer() = &vkR;
    Engine::GetRenderer()->Initialize(context);


    Scene* scene = new Scene();


    scene -> Run();
    scene -> Update();
    scene -> Exit();

    Engine::Exit();
    context.Destroy();

    std::cout<<"FINAL\n";

    return 0;
}