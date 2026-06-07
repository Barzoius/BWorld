#include "VkRenderer.hpp"
#include <memory>


#include "Bindables/VertexSystem.hpp"



void VkRenderer::Initialize(Context& context) 
{
    std::cout << "VkRenderer initialized\n";

    std::string frag = "Shaders/base1.frag.spv";
    std::string vert = "Shaders/base1.vert.spv";


    vertex = std::make_unique<Shader<ShaderType::VERTEX>>(m_vkContext, vert);
    fragment = std::make_unique<Shader<ShaderType::FRAGMENT>>(m_vkContext, frag);

    create_swapchain();

    create_GFX_pipeline();


    create_sync_resources();
    create_frame_data_v2();

}



void VkRenderer::RenderFrame() 
{
    DVS::VertexLayout vl;
    vl.append(DVS::VertexLayout::Position3D).append(DVS::VertexLayout::Float3Color);
    DVS::VertexBuffer vb(std::move(vl));
    vb.emplace_back(DVS::VKFLOAT3{1.0f, 1.0f, 5.0f}, DVS::VKFLOAT3{1.0f, 1.0f, 5.0f});
    auto pos = vb[0].attribute<DVS::VertexLayout::Position3D>();
    std::cout<<pos.z;
    render_with_new_sync();
}

void VkRenderer::Shutdown() {
    std::cout << "VkRenderer shutdown\n";
    
    vkDeviceWaitIdle(m_vkContext.get_device().get());


    if (timelineSmph)
	{
		vkDestroySemaphore(m_vkContext.get_device().get(), timelineSmph, nullptr);
	}

    for (auto &data : m_frameResources)
	{
		vkDestroySemaphore(m_vkContext.get_device().get(), data.s_imgAcquiredSmph, nullptr);
		vkDestroyCommandPool(m_vkContext.get_device().get(), data.s_commandPool, nullptr); 
	}


    for (VkSemaphore &semaphore : m_renderCompleteSmphs)
	{
		vkDestroySemaphore(m_vkContext.get_device().get(), semaphore, nullptr);
	}
	m_renderCompleteSmphs.clear();

    clean_swapchain_v2();

    if (gfxPipeline)
    {
        gfxPipeline->Destroy();
        gfxPipeline.reset();
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


void VkRenderer::create_sync_resources()
{
    VkSemaphoreTypeCreateInfo smphTypeInfo
    {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
        .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
        .initialValue = MAX_FRAMES_IN_FLIGHT
    };

    VkSemaphoreCreateInfo smphInfo
    {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = &smphTypeInfo
    };

    if(vkCreateSemaphore(m_vkContext.get_device().get(), &smphInfo, nullptr, &timelineSmph) != VK_SUCCESS)
    {
            throw std::runtime_error("failed to create timeline semaphore!");
    }

    for(frameData &data : m_frameResources)
    {
        VkSemaphoreCreateInfo smphInfo
        {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
        };
        if(vkCreateSemaphore(m_vkContext.get_device().get(), &smphInfo, nullptr, &data.s_imgAcquiredSmph) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create semaphore for image acquisition!");

        }
    }

    m_renderCompleteSmphs.resize(swapchain.get()->get_images().size());
    for(VkSemaphore& smph : m_renderCompleteSmphs)
    {
        VkSemaphoreCreateInfo smphInfo{ .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
                if(vkCreateSemaphore(m_vkContext.get_device().get(), &smphInfo, nullptr, &smph) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create semaphore for render completion!");

        }
    }
}


void VkRenderer::create_frame_data_v2()
{
    for(frameData &data : m_frameResources)
    {
        vkutil::QueueFamilyIndices queueFamilyIndices = m_vkContext.get_device().get_device_indices();
        VkCommandPoolCreateInfo poolInfo
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .queueFamilyIndex = queueFamilyIndices.graphicsFamily.value()
        };

        if(vkCreateCommandPool(m_vkContext.get_device().get(), &poolInfo, nullptr, &data.s_commandPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create command pools!");

        }

        VkCommandBufferAllocateInfo cmdAllocInfo
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = data.s_commandPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };

        if(vkAllocateCommandBuffers(m_vkContext.get_device().get(), &cmdAllocInfo, &data.s_commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create command buffers!");
        }
    }
}

void VkRenderer::clean_new_sync()
{
    if(timelineSmph)
        vkDestroySemaphore(m_vkContext.get_device().get(), timelineSmph, nullptr);
    
    for(frameData &data : m_frameResources)
    {
        vkDestroySemaphore(m_vkContext.get_device().get(), data.s_imgAcquiredSmph, nullptr);
        vkDestroyCommandPool(m_vkContext.get_device().get(), data.s_commandPool, nullptr);

    }
}

void VkRenderer::render_with_new_sync()
{
    
    if(m_swapchainRecreation)
    {
        vkDeviceWaitIdle(m_vkContext.get_device().get());
        clean_swapchain_v2();
        create_swapchain();
        m_swapchainRecreation = false;
    }

    const uint32_t frameDataIndex = currentFrame++ % MAX_FRAMES_IN_FLIGHT;
    const uint64_t signalValue = nextSignalValue++;
    const uint64_t waitValue = signalValue - MAX_FRAMES_IN_FLIGHT;

    VkSemaphoreWaitInfo waitInfo
    {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
        .semaphoreCount = 1,
        .pSemaphores = &timelineSmph,
        .pValues = &waitValue
    };
    vkWaitSemaphores(m_vkContext.get_device().get(), &waitInfo, UINT64_MAX);

    frameData &data = m_frameResources[frameDataIndex];
    vkResetCommandPool(m_vkContext.get_device().get(), data.s_commandPool, 0);

    VkSemaphore imgAquireSemph = data.s_imgAcquiredSmph;

    uint32_t imageIndex = 0;
    VkResult aquireResult = vkAcquireNextImageKHR(m_vkContext.get_device().get(), swapchain.get()->get_handle(), UINT64_MAX, imgAquireSemph, VK_NULL_HANDLE, &imageIndex);

    if(aquireResult == VK_ERROR_OUT_OF_DATE_KHR)
    {
        m_swapchainRecreation = true;
        return;
    }
    else if(aquireResult == VK_SUBOPTIMAL_KHR)
    {
        m_swapchainRecreation = true;
    }

    VkCommandBufferBeginInfo cmdBeginInfo
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };
    
    vkBeginCommandBuffer(data.s_commandBuffer, &cmdBeginInfo);

    std::vector<VkImageMemoryBarrier2> layoutBarriers
	{
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
			.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
			.srcAccessMask = 0,
			.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
			.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.image = swapchain.get() -> get_images()[imageIndex],
			.subresourceRange
			{
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1,
			}
		},
		// {
		// 	.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
		// 	.srcStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT,
		// 	.srcAccessMask = 0,
		// 	.dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
		// 	.dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		// 	.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		// 	.newLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
		// 	.image = depthImage,
		// 	.subresourceRange
		// 	{
		// 		.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
		// 		.baseMipLevel = 0,
		// 		.levelCount = 1,
		// 		.baseArrayLayer = 0,
		// 		.layerCount = 1,
		// 	}
		// }
    };

    VkDependencyInfo depInfo
    {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .imageMemoryBarrierCount = static_cast<uint32_t>(layoutBarriers.size()),
        .pImageMemoryBarriers = layoutBarriers.data()
    };
    // std::cout<<"BARR1-before\n";

    vkCmdPipelineBarrier2(data.s_commandBuffer, &depInfo);
    // std::cout<<"BARR1\n";
    VkRenderingAttachmentInfo colorAttInfo
    {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView = swapchain.get()->get_image_views()[imageIndex],
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue{.color{0.01f, 0.01f, 0.01f, 1.0f}}
    };

	VkRenderingAttachmentInfo depthAttInfo
	{
		.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
		.imageView = depthImageView,
		.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR, 
		.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.clearValue{.depthStencil{1.0f, 0}}
	};

    VkRenderingInfo renderInfo
    {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea
        {
            .offset{.x = 0, .y =0},
            .extent{.width = static_cast<uint32_t>(swapchainContext.width), .height = static_cast<uint32_t>(swapchainContext.height)}
        },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttInfo,
        .pDepthAttachment = &depthAttInfo
    };

    vkCmdBeginRendering(data.s_commandBuffer, &renderInfo);
    {
        VkViewport viewport 
        {
            .x = 0, .y = 0,
            .width = static_cast<float>(swapchainContext.width),
            .height = static_cast<float>(swapchainContext.height)

        };
        vkCmdSetViewport(data.s_commandBuffer, 0, 1, &viewport);

        VkRect2D scissor
        {
            .offset{.x = 0, .y =0},
            .extent{.width = static_cast<uint32_t>(swapchainContext.width), .height = static_cast<uint32_t>(swapchainContext.height)}
        };

        vkCmdSetScissor(data.s_commandBuffer, 0, 1, &scissor);

        vkCmdBindPipeline(data.s_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, gfxPipeline.get()->get_handle());

        vkCmdDraw(data.s_commandBuffer, 3, 1, 0, 0);
    };
    vkCmdEndRendering(data.s_commandBuffer);

    VkImageMemoryBarrier2 presentLayoutBarrier
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
		.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		.dstStageMask = VK_PIPELINE_STAGE_2_NONE,
		.dstAccessMask = 0,
		.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		.image = swapchain.get() -> get_images()[imageIndex],
		.subresourceRange
		{
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1,
		}
	};
	VkDependencyInfo presentDepInfo
	{
		.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
		.imageMemoryBarrierCount = 1,
		.pImageMemoryBarriers = &presentLayoutBarrier
	};
    // std::cout<<"BARR2-before\n";

	vkCmdPipelineBarrier2(data.s_commandBuffer, &presentDepInfo);
    // std::cout<<"BARR2\n";


	vkEndCommandBuffer(data.s_commandBuffer);

    VkSemaphoreSubmitInfo imageAcquireWaitInfo
	{
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
		.semaphore = data.s_imgAcquiredSmph,
		.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT 
	};
	
	std::vector<VkSemaphoreSubmitInfo> semaphoreSignals
	{
		{ // render work completion signal
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
			.semaphore = m_renderCompleteSmphs[imageIndex],
			.stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT
		},
		{ // entire frame is completed (timeline)
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
			.semaphore = timelineSmph,
			.value = signalValue,
			.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT
		}
	};
	VkCommandBufferSubmitInfo cmdSubmitInfo
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
		.commandBuffer = data.s_commandBuffer,
	};
	VkSubmitInfo2 submitInfo
	{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
		.waitSemaphoreInfoCount = 1,
		.pWaitSemaphoreInfos = &imageAcquireWaitInfo, 
		.commandBufferInfoCount = 1,
		.pCommandBufferInfos = &cmdSubmitInfo,
		.signalSemaphoreInfoCount = static_cast<uint32_t>(semaphoreSignals.size()),
		.pSignalSemaphoreInfos = semaphoreSignals.data()
	};
	vkQueueSubmit2(m_vkContext.get_device().graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);

    VkSwapchainKHR swapchainHandle = swapchain.get()->get_handle();
	// present the image
	VkPresentInfoKHR presentInfo{
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &m_renderCompleteSmphs[imageIndex], 
		.swapchainCount = 1,
		.pSwapchains = &swapchainHandle,
		.pImageIndices = &imageIndex,
		.pResults = nullptr
	};

	vkQueuePresentKHR(m_vkContext.get_device().graphicsQueue, &presentInfo);

}

void VkRenderer::clean_swapchain_v2()
{
    swapchain.get()->Destroy();
}




void VkRenderer::recreate_swapchain_v2()
{

}




