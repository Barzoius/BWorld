// VkRenderer.cpp
#include "VkRenderer.hpp"
#include <iostream> 

void VkRenderer::Initialize(std::vector<const char*>) {
    std::cout << "VkRenderer initialized\n";
    
}

void VkRenderer::RenderFrame() {
    std::cout << "VkRenderer rendering frame\n";
   
}

void VkRenderer::Shutdown() {
    std::cout << "VkRenderer shutdown\n";

}