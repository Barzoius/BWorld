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
    CreateRenderPass();
    CreateGFXPipeline();
    createFramebuffers();
    CreateCommandPool();

}

void VkRenderer::RenderFrame() {
    
   //vkWaitForFences(vkContext.getDevice().get(), 1, &inFlightFence, VK_TRUE, UINT64_MAX);
}

void VkRenderer::Shutdown() {
    std::cout << "VkRenderer shutdown\n";
    
    vkDeviceWaitIdle(vkContext.getDevice().get());

    // vkDestroySemaphore(vkContext.getDevice().get(), imageAvailableSemaphore, nullptr);
    // vkDestroySemaphore(vkContext.getDevice().get(), renderFinishedSemaphore, nullptr);
    // vkDestroyFence(vkContext.getDevice().get(), inFlightFence, nullptr);

    for (auto framebuffer : swapChainFramebuffers) {
        vkDestroyFramebuffer(vkContext.getDevice().get(), framebuffer, nullptr);
    }
    swapChainFramebuffers.clear();

    if (gfxPipeline)
    {
        gfxPipeline->Destroy();
        gfxPipeline.reset();
    }
    if (renderPass)
    {
        renderPass->Destroy();
        renderPass.reset();
    }
    if (vertex)
    {
        vertex->Destroy();
        vertex.reset();
    }
    if (fragment)
    {
        fragment->Destroy();
        fragment.reset();
    }
    if (swapchain)
    {
        swapchain->Destroy();
        swapchain.reset();
    }
    if(commandPool)
    {
        commandPool->Destroy();
        commandPool.reset();
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

    swapchainContext.extent = swapchain -> getExtent();
    swapchainContext.imageFormat = swapchain -> getImageFormat();
    swapchainContext.width = swapchain -> getWidth();
    swapchainContext.height = swapchain -> getHeight();

}

void VkRenderer::CreateRenderPass()
{
    renderPass = std::make_unique<RenderPass>(vkContext, swapchainContext);
    renderPass.get()->createRenderPass();
}

void VkRenderer::CreateGFXPipeline()
{
    gfxPipeline = std::make_unique<GraphicsPipeline>(vkContext, swapchainContext);
    gfxPipeline.get()->createPipeline(*vertex, *fragment, *renderPass);
}

void VkRenderer::createFramebuffers()
{
    size_t size = swapchain.get()->getImageViews().size();
    swapChainFramebuffers.resize(size);

    for(size_t i = 0; i < size; i++)
    {
        VkImageView attachments[] = {swapchain.get()->getImageViews()[i]};
    
        VkFramebufferCreateInfo frameBufferInfo{};
        frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameBufferInfo.renderPass = renderPass.get()->getRenderPass();
        frameBufferInfo.attachmentCount = 1;
        frameBufferInfo.pAttachments = attachments;
        frameBufferInfo.width = swapchainContext.width;
        frameBufferInfo.height = swapchainContext.height;
        frameBufferInfo.layers = 1;

        if (vkCreateFramebuffer(vkContext.getDevice().get(), &frameBufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }

    std::cout<<"FrameBuffers Created\n";
}

void VkRenderer::CreateCommandPool()
{
    commandPool = std::make_unique<VulkanCommandPool>(vkContext, swapchainContext, *renderPass, swapChainFramebuffers);

    commandPool.get()->Initialize();
}


void VkRenderer::createSyncObjects()
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateSemaphore(vkContext.getDevice().get(), &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
    vkCreateSemaphore(vkContext.getDevice().get(), &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
    vkCreateFence(vkContext.getDevice().get(), &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS) {
    throw std::runtime_error("failed to create semaphores!");
    }

}
