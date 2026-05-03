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

    createCommandBuffer();

    createSyncObjects();

}

void VkRenderer::RenderFrame() {
    
    vkWaitForFences(vkContext.get_device().get(), 1, &inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(vkContext.get_device().get(), 1, &inFlightFence);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(vkContext.get_device().get(), swapchain.get()->getHandle(), UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    vkResetCommandBuffer(commandBuffer, 0);
    recordCommandBuffer(imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(vkContext.get_device().graphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { swapchain.get()->getHandle() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    presentInfo.pResults = nullptr; // Optional

    vkQueuePresentKHR(vkContext.get_device().presentQueue, &presentInfo);
}

void VkRenderer::Shutdown() {
    std::cout << "VkRenderer shutdown\n";
    
    vkDeviceWaitIdle(vkContext.get_device().get());

    vkDestroySemaphore(vkContext.get_device().get(), imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(vkContext.get_device().get(), renderFinishedSemaphore, nullptr);
    vkDestroyFence(vkContext.get_device().get(), inFlightFence, nullptr);

    for (auto framebuffer : swapChainFramebuffers) {
        vkDestroyFramebuffer(vkContext.get_device().get(), framebuffer, nullptr);
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

    swapchain = std::make_unique<VulkanSwapchain>(vkContext.get_device().get(), vkContext.get_device().getPhyD(),  vkContext.get_instance().getSurfaceHandle(), 
                                                         vkContext.get_instance().getResolution().width, 
                                                         vkContext.get_instance().getResolution().height);
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

        if (vkCreateFramebuffer(vkContext.get_device().get(), &frameBufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) 
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

void VkRenderer::createCommandBuffer()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool.get()->get_handle();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(vkContext.get_device().get(), &allocInfo, &commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    std::cout<<"Command Buffer Created\n";
}

void VkRenderer::recordCommandBuffer(uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass.get()->getRenderPass();
    renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];

    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapchainContext.extent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,  gfxPipeline.get()->get_handle());

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapchainContext.width);
    viewport.height = static_cast<float>(swapchainContext.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapchainContext.extent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }

}


void VkRenderer::createSyncObjects()
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateSemaphore(vkContext.get_device().get(), &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
    vkCreateSemaphore(vkContext.get_device().get(), &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
    vkCreateFence(vkContext.get_device().get(), &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS) {
    throw std::runtime_error("failed to create semaphores!");
    }

}
