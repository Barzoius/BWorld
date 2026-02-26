#include "VkRenderer.hpp"

VkRenderer::VkRenderer() : instance(), device(instance)
{

}

void VkRenderer::Initialize(std::vector<const char*> exts) {
    std::cout << "VkRenderer initialized\n";
    
    instance.Initialize(exts);
    device.Initialize();

}

void VkRenderer::RenderFrame() {
    std::cout << "VkRenderer rendering frame\n";
   
}

void VkRenderer::Shutdown() {
    std::cout << "VkRenderer shutdown\n";
}


