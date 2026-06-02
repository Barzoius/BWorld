#include "VkRenderer.hpp"
#include <memory>



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

    m_renderFinishedSmph.resize(swapchain->get_image_views().size(), VK_NULL_HANDLE);
    m_imgAvailableSmph.resize(MAX_FRAMES_IN_FLIGHT, VK_NULL_HANDLE);
    m_commandBuffer.resize(MAX_FRAMES_IN_FLIGHT);
    m_inFlightFence.resize(MAX_FRAMES_IN_FLIGHT, VK_NULL_HANDLE);
    create_frame_data();

}



void VkRenderer::RenderFrame() 
{
    VkResult result_fence = vkWaitForFences(m_vkContext.get_device().get(), 1, &m_inFlightFence[currentFrame], VK_TRUE, UINT64_MAX);

    if (result_fence == VK_TIMEOUT)
    {
        throw std::runtime_error("Fence wait timeout (GPU hang or deadlock)");
    }

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(m_vkContext.get_device().get(), swapchain.get()->get_handle(), UINT64_MAX, m_imgAvailableSmph[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR /*|| m_framebufferResized*/) 
    {
        vkDestroySemaphore(m_vkContext.get_device().get(), m_imgAvailableSmph[currentFrame], nullptr);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        if (vkCreateSemaphore(m_vkContext.get_device().get(), &semaphoreInfo, nullptr, &m_imgAvailableSmph[currentFrame]) != VK_SUCCESS)
            throw std::runtime_error("failed to create semaphore!");


        //m_framebufferResized = false;
        recreate_swapcahin();
        return;
        
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }



    vkResetFences(m_vkContext.get_device().get(), 1, &m_inFlightFence[currentFrame]);


    vkResetCommandBuffer(m_commandBuffer[currentFrame], 0);
    recordCommandBuffer(m_commandBuffer[currentFrame], imageIndex);



    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {m_imgAvailableSmph[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandBuffer[currentFrame];

    VkSemaphore signalSemaphores[] = {m_renderFinishedSmph[imageIndex]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(m_vkContext.get_device().graphicsQueue, 1, &submitInfo, m_inFlightFence[currentFrame]) != VK_SUCCESS) 
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

    for(size_t i = 0; i < swapchain.get()->get_image_views().size(); i++)
    {
        vkDestroySemaphore(m_vkContext.get_device().get(), m_renderFinishedSmph[i], nullptr);
    }

    
    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(m_vkContext.get_device().get(), m_imgAvailableSmph[i], nullptr);
        vkDestroyFence(m_vkContext.get_device().get(), m_inFlightFence[i], nullptr);
    }

    clean_swapchain();

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
    if(commandPool)
    {
        commandPool->Destroy();
        commandPool.reset();
    }


}

 void VkRenderer::UpdateResolution(const Resolution& res)
 {
    swapchain.get()->update_resolution(res.width, res.height);
    m_vkContext.update_instance_resolution(res);
    
    std::cout<<"UPDATE RESOLUTION RENDERER\n";
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

void VkRenderer::recreate_swapcahin()
{
    vkDeviceWaitIdle(m_vkContext.get_device().get());

    clean_swapchain();

    create_swapchain();
    create_framebuffers();

}

void VkRenderer::clean_swapchain()
{
    for (auto framebuffer : swapChainFramebuffers) {
        vkDestroyFramebuffer(m_vkContext.get_device().get(), framebuffer, nullptr);
    }
    swapChainFramebuffers.clear();

    swapchain.get()->Destroy();

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


    if(vkAllocateCommandBuffers(m_vkContext.get_device().get(), &allocInfo, m_commandBuffer.data())!= VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers");
    }


    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for(size_t i = 0; i < swapchain.get()->get_image_views().size(); i++)
    {
                vkCreateSemaphore(m_vkContext.get_device().get(), 
                          &semaphoreInfo, nullptr,
                          &m_renderFinishedSmph[i]); 
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if(
        vkCreateSemaphore(m_vkContext.get_device().get(), 
                          &semaphoreInfo, nullptr, 
                          &m_imgAvailableSmph[i]) != VK_SUCCESS ||
        vkCreateFence(m_vkContext.get_device().get(), 
                      &fenceInfo, nullptr, 
                      &m_inFlightFence[i]) != VK_SUCCESS
        )
        {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }

}



void VkRenderer::create_gpu_timeline()
{
    VkSemaphoreTypeCreateInfo timelineInfo{};
    timelineInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    timelineInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
    timelineInfo.initialValue = 0;

    VkSemaphoreCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    createInfo.pNext = &timelineInfo;

    vkCreateSemaphore(m_vkContext.get_device().get(), &createInfo, nullptr, &m_gpuTimeline);
}
