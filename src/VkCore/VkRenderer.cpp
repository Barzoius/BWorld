#include "VkRenderer.hpp"

#include <filesystem>
#include <fstream>
#include <memory>

namespace fs = std::filesystem;

void VkRenderer::Initialize(Context& context) 
{
    std::cout << "VkRenderer initialized\n";

    std::string frag = "Shaders/base1.frag.spv";
    std::string vert = "Shaders/base1.vert.spv";



    vertex = std::make_unique<Shader<ShaderType::VERTEX>>(vkContext, vert);
    fragment = std::make_unique<Shader<ShaderType::FRAGMENT>>(vkContext, frag);

    CreateSwapChain();

}

void VkRenderer::RenderFrame() {
    std::cout << "VkRenderer rendering frame\n";
   
}

void VkRenderer::Shutdown() {
    std::cout << "VkRenderer shutdown\n";
    
    renderPass.Destroy();
    vertex ->Destroy();
    fragment -> Destroy();

    if (swapchain)
    {
        vkDeviceWaitIdle(vkContext.getDevice().get());

        swapchain->Destroy();
        swapchain.reset();
    }

}

 void VkRenderer::UpdateResolution(const Resolution& res)
 {

 }

void VkRenderer::CreateSwapChain()
{

    swapchain = std::make_unique<VulkanSwapchain>(vkContext.getDevice().get(), vkContext.getDevice().getPhyD(),  vkContext.getInstance().getSurfaceHandle(), 
                                                         vkContext.getInstance().getResolution().width, 
                                                         vkContext.getInstance().getResolution().height);
    swapchain -> createSwapChain();
    swapchain -> createSwapChainImageViews();

}
