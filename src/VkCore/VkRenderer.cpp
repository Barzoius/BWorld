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



    vertex = std::make_unique<Shader<ShaderType::VERTEX>>(m_vkContext, vert);
    fragment = std::make_unique<Shader<ShaderType::FRAGMENT>>(m_vkContext, frag);

    create_swapchain();

    create_renderpass();
    create_GFX_pipeline();

    create_framebuffers();

    create_commandpool();

    m_imagesInFlightSmph.resize(swapchain->get_image_views().size(), VK_NULL_HANDLE);
    create_frame_data();


}



void VkRenderer::RenderFrame() 
{
    vkWaitForFences(m_vkContext.get_device().get(), 1, &frameResources[currentFrame].m_inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(m_vkContext.get_device().get(), 1, &frameResources[currentFrame].m_inFlightFence);



    uint32_t imageIndex;
    vkAcquireNextImageKHR(m_vkContext.get_device().get(), swapchain.get()->get_handle(), UINT64_MAX, frameResources[currentFrame].m_imgAvailableSmph, VK_NULL_HANDLE, &imageIndex);

    
    // VkSemaphore waitImageSemaphore = m_imagesInFlightSmph[imageIndex];

    vkResetCommandBuffer(frameResources[currentFrame].m_commandBuffer, 0);
    recordCommandBuffer(frameResources[currentFrame].m_commandBuffer, imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {frameResources[currentFrame].m_imgAvailableSmph};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &frameResources[currentFrame].m_commandBuffer;

    VkSemaphore signalSemaphores[] = {frameResources[currentFrame].m_renderFinishedSmph};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(m_vkContext.get_device().graphicsQueue, 1, &submitInfo, frameResources[currentFrame].m_inFlightFence) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { swapchain.get()->get_handle() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    presentInfo.pResults = nullptr; // Optional

    vkQueuePresentKHR(m_vkContext.get_device().presentQueue, &presentInfo);

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VkRenderer::Shutdown() {
    std::cout << "VkRenderer shutdown\n";
    
    vkDeviceWaitIdle(m_vkContext.get_device().get());

    for(auto frame : frameResources)
    {
        vkDestroySemaphore(m_vkContext.get_device().get(), frame.m_renderFinishedSmph, nullptr);
        vkDestroySemaphore(m_vkContext.get_device().get(), frame.m_imgAvailableSmph, nullptr);
        vkDestroyFence(m_vkContext.get_device().get(), frame.m_inFlightFence, nullptr);
    }

    for (auto framebuffer : swapChainFramebuffers) {
        vkDestroyFramebuffer(m_vkContext.get_device().get(), framebuffer, nullptr);
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

void VkRenderer::create_swapchain()
{

    swapchain = std::make_unique<VulkanSwapchain>(m_vkContext.get_device().get(), m_vkContext.get_device().getPhyD(),  m_vkContext.get_instance().get_surface_handle(), 
                                                         m_vkContext.get_instance().get_resolution().width, 
                                                         m_vkContext.get_instance().get_resolution().height);
    swapchain -> createSwapChain();
    swapchain -> create_swapchain_image_views();

    swapchainContext.extent = swapchain -> get_extent();
    swapchainContext.imageFormat = swapchain -> get_image_format();
    swapchainContext.width = swapchain -> get_width();
    swapchainContext.height = swapchain -> get_height();

}

void VkRenderer::create_renderpass()
{
    renderPass = std::make_unique<RenderPass>(m_vkContext, swapchainContext);
    renderPass.get()->createRenderPass();
}

void VkRenderer::create_GFX_pipeline()
{
    gfxPipeline = std::make_unique<GraphicsPipeline>(m_vkContext, swapchainContext);
    gfxPipeline.get()->createPipeline(*vertex, *fragment, *renderPass);
}

void VkRenderer::create_framebuffers()
{
    size_t size = swapchain.get()->get_image_views().size();
    swapChainFramebuffers.resize(size);

    for(size_t i = 0; i < size; i++)
    {
        VkImageView attachments[] = {swapchain.get()->get_image_views()[i]};
    
        VkFramebufferCreateInfo frameBufferInfo{};
        frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameBufferInfo.renderPass = renderPass.get()->getRenderPass();
        frameBufferInfo.attachmentCount = 1;
        frameBufferInfo.pAttachments = attachments;
        frameBufferInfo.width = swapchainContext.width;
        frameBufferInfo.height = swapchainContext.height;
        frameBufferInfo.layers = 1;

        if (vkCreateFramebuffer(m_vkContext.get_device().get(), &frameBufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }

    std::cout<<"FrameBuffers Created\n";
}

void VkRenderer::create_commandpool()
{
    commandPool = std::make_unique<VulkanCommandPool>(m_vkContext, swapchainContext, *renderPass, swapChainFramebuffers);

    commandPool.get()->Initialize();
}


void VkRenderer::recordCommandBuffer(VkCommandBuffer& buffer, uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(buffer, &beginInfo) != VK_SUCCESS) {
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

    vkCmdBeginRenderPass(buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,  gfxPipeline.get()->get_handle());

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapchainContext.width);
    viewport.height = static_cast<float>(swapchainContext.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(buffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapchainContext.extent;
    vkCmdSetScissor(buffer, 0, 1, &scissor);

    vkCmdDraw(buffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(buffer);
    if (vkEndCommandBuffer(buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }

}




void VkRenderer::create_frame_data()
{

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool.get()->get_handle();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

    std::vector<VkCommandBuffer> commandBuffers(MAX_FRAMES_IN_FLIGHT); // vk expects continous memory layout
    if(vkAllocateCommandBuffers(m_vkContext.get_device().get(), &allocInfo, commandBuffers.data())!= VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers");
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        frameResources[i].m_commandBuffer = commandBuffers[i];
    }

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (auto& frame : frameResources)
    {
        if(
        vkCreateSemaphore(m_vkContext.get_device().get(), 
                          &semaphoreInfo, nullptr, 
                          &frame.m_imgAvailableSmph) != VK_SUCCESS ||
        vkCreateSemaphore(m_vkContext.get_device().get(), 
                          &semaphoreInfo, nullptr,
                          &frame.m_renderFinishedSmph) != VK_SUCCESS ||
        vkCreateFence(m_vkContext.get_device().get(), 
                      &fenceInfo, nullptr, 
                      &frame.m_inFlightFence) != VK_SUCCESS
        )
        {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }

}
