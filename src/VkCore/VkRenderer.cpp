#include "VkRenderer.hpp"

VkRenderer::VkRenderer() : instance(), device(instance)
{

}

void VkRenderer::Initialize(const std::vector<const char*>& exts, 
                            const SurfaceInfo& surface, 
                            const Resolution& resolution) 
{
    std::cout << "VkRenderer initialized\n";
    
    instance.Initialize(exts, surface, resolution);
    device.Initialize();

}

void VkRenderer::RenderFrame() {
    std::cout << "VkRenderer rendering frame\n";
   
}

void VkRenderer::Shutdown() {
    std::cout << "VkRenderer shutdown\n";
}

 void VkRenderer::UpdateResolution(const Resolution& res)
 {

 }


