#include "VkRenderer.hpp"

VkRenderer::VkRenderer() : instance(), physicalDevice(instance), logicalDevice(physicalDevice)
{

}

void VkRenderer::Initialize(std::vector<const char*> exts) {
    std::cout << "VkRenderer initialized\n";
    instance.Initialize(exts);
    physicalDevice.Initialize();
    logicalDevice.Initialize();
}

void VkRenderer::RenderFrame() {
    std::cout << "VkRenderer rendering frame\n";
   
}

void VkRenderer::Shutdown() {
    std::cout << "VkRenderer shutdown\n";
}


