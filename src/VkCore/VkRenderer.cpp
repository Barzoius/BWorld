#include "VkRenderer.hpp"
#include <unordered_set>
#include <algorithm>

void VkRenderer::Initialize(std::vector<const char*> exts) {
    std::cout << "VkRenderer initialized\n";
    instance.Initialize(exts);
}

void VkRenderer::RenderFrame() {
    std::cout << "VkRenderer rendering frame\n";
   
}

void VkRenderer::Shutdown() {
    std::cout << "VkRenderer shutdown\n";

    // if (enableValidationLayers) 
    //     DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    
    // vkDestroyInstance(instance, nullptr);

}


