#include "VkRenderer.hpp"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

void VkRenderer::Initialize(Context& context) 
{
    std::cout << "VkRenderer initialized\n";

    std::string frag = "Shaders/base1.frag.spv";
    std::string vert = "Shaders/base1.vert.spv";



    vertex = std::make_unique<Shader<ShaderType::VERTEX>>(vkContext, vert);
    fragment = std::make_unique<Shader<ShaderType::FRAGMENT>>(vkContext, frag);

    renderPass.createRenderPass();
   
    // gfxPipelines.reserve(1);
    // gfxPipelines.push_back(std::make_unique<GraphicsPipeline>());
    // gfxPipelines[0]->createPipeline(device, *vertex, *fragment, renderPass);

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

void VkRenderer::CreateSwapChain()
{
    // swapchain = std::make_unique<VulkanSwapchain>(handle, phyD, instance.getSurfaceHandle(), 
    //                                                     instance.getResolution().width, 
    //                                                     instance.getResolution().height);
    // swapchain -> createSwapChain();
    // swapchain -> createSwapChainImageViews();

}
