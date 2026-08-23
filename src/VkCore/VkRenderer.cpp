#include "VkRenderer.hpp"
#include <memory>


// #include "Resources/VertexSystem.hpp"

#include "VertexSystem.hpp"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

void VkRenderer::Initialize(Context& context) 
{
    std::cout << "VkRenderer initialized\n";

    std::string frag = "Shaders/base1.frag.spv";
    std::string vert = "Shaders/base1.vert.spv";


    vertex = std::make_unique<Shader<ShaderType::VERTEX>>(m_vkContext, vert);
    fragment = std::make_unique<Shader<ShaderType::FRAGMENT>>(m_vkContext, frag);

 
    create_swapchain();

    construct_vertex_buffer();

    create_GFX_pipeline();



    create_sync_resources();
    create_frame_data_v2();

}

void VkRenderer::construct_vertex_buffer()
{
    
    DVS::VertexLayout vl;
    vl.append(DVS::VertexLayout::Position2D).append(DVS::VertexLayout::Float3Color);
    DVS::VertexBuffer vb(std::move(vl));
    vb.emplace_back(DVS::VKFLOAT2{-0.5f, -0.5f}, DVS::VKFLOAT3{1.0f, 1.0f, 1.0f});
    vb.emplace_back(DVS::VKFLOAT2{0.5f, -0.5f}, DVS::VKFLOAT3{0.0f, 1.0f, 0.0f});
    vb.emplace_back(DVS::VKFLOAT2{0.5f, 0.5f}, DVS::VKFLOAT3{0.0f, 0.0f, 1.0f});
    vb.emplace_back(DVS::VKFLOAT2{-0.5f, 0.5f}, DVS::VKFLOAT3{0.0f, 1.0f, 1.0f});


    input_vertex_buffers.push_back(vb);


    vertex_buffer = create_vertex_buffer_with_staging(m_vkContext.transfer_sys, vb, m_vkContext.get_allocator());

    //std::cout<<"VERTEX COUNT: "<<input_vertex_buffers[0].get_size_in_vertices()<<"\n";

    index_buffer = create_index_buffer(m_vkContext.transfer_sys, indices, m_vkContext.get_allocator());
    
    // uniform_buffers.resize(MAX_FRAMES_IN_FLIGHT);
    // for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    // {
    //     buffer uniform = create_uniform_buffer(m_vkContext.get_allocator());
    //     uniform_buffers.push_back(uniform);
    // }
}

void VkRenderer::update_uniform_buffer(uint32_t currentImage)
{
    // static auto startTime = std::chrono::high_resolution_clock::now();

    // auto currentTime = std::chrono::high_resolution_clock::now();
    // float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    // UniformBufferObject ubo{};
    // ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    // ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    // ubo.proj = glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float) swapChainExtent.height, 0.1f, 10.0f);
    // ubo.proj[1][1] *= -1;

    // upload_to_buffer(
    //     m_vkContext.get_allocator(),
    //     uniform_buffers[currentImage],
    //     &ubo,
    //     sizeof(ubo)
    // );
    
}




void VkRenderer::RenderFrame() 
{
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


    delete_buffer(vertex_buffer, m_vkContext.get_allocator());
    delete_buffer(index_buffer, m_vkContext.get_allocator());
    // for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    // {
    //     delete_buffer(uniform_buffers[i], m_vkContext.get_allocator());
    // }


    clean_swapchain_v2();

    //sterge
    vkDestroyDescriptorSetLayout(m_vkContext.get_device().get(), descriptorSetLayout, nullptr);

    if (gfxPipeline)
    {
        gfxPipeline->destroy();
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

}

 void VkRenderer::UpdateResolution(const Resolution& res)
 {
    swapchain.get()->update_resolution(res.width, res.height);
    m_vkContext.update_instance_resolution(res);
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
    swapchainContext.images = swapchain -> get_images().size();

}




void VkRenderer::create_GFX_pipeline()
{
    gfxPipeline = std::make_unique<GraphicsPipeline>(m_vkContext, swapchainContext);

    GraphicsPipelineDesc desc{};


    VkVertexInputBindingDescription bindDesc{};
    bindDesc.binding = 0; 
    bindDesc.stride = (uint32_t)input_vertex_buffers[0].get_layout().get_size();
    bindDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    desc.vertLayout.bindDesc = bindDesc;



    std::vector<VkVertexInputAttributeDescription> descs;
    descs.resize(input_vertex_buffers[0].get_layout().get_count());
    for(int i = 0; i < descs.size(); i++)
    {
        descs[i].binding = 0; // modulate this later
        descs[i].location = i;
        descs[i].format = get_vertex_buffer_format(input_vertex_buffers[0].get_layout().resolve_by_index((size_t)i).get_type());
        descs[i].offset = input_vertex_buffers[0].get_layout().resolve_by_index((size_t)i).get_offset();
        
    }

    desc.vertLayout.attrDescs = descs;

    desc.vertShader = vertex.get();
    desc.fragShader = fragment.get();

    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr; // Optional
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    if (vkCreateDescriptorSetLayout(m_vkContext.get_device().get(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }

    gfxPipeline.get()->create_pipeline(desc, descriptorSetLayout);
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
            .queueFamilyIndex = queueFamilyIndices.s_graphics.value()
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

    vkCmdPipelineBarrier2(data.s_commandBuffer, &depInfo);

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


        VkBuffer vertexBuffers[] = {vertex_buffer.s_handle};

        VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(data.s_commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(data.s_commandBuffer, index_buffer.s_handle, 0, VK_INDEX_TYPE_UINT16);


        vkCmdDrawIndexed(data.s_commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
    


    };
    vkCmdEndRendering(data.s_commandBuffer);

    VkImageMemoryBarrier2 presentLayoutBarrier{
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
	
    VkDependencyInfo presentDepInfo{
		.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
		.imageMemoryBarrierCount = 1,
		.pImageMemoryBarriers = &presentLayoutBarrier
	};

	vkCmdPipelineBarrier2(data.s_commandBuffer, &presentDepInfo);


	vkEndCommandBuffer(data.s_commandBuffer);

    VkSemaphoreSubmitInfo imageAcquireWaitInfo{
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
		.semaphore = data.s_imgAcquiredSmph,
		.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT 
	};
	
	std::vector<VkSemaphoreSubmitInfo> semaphoreSignals{
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

	VkCommandBufferSubmitInfo cmdSubmitInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
		.commandBuffer = data.s_commandBuffer,
	};


	VkSubmitInfo2 submitInfo{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
		.waitSemaphoreInfoCount = 1,
		.pWaitSemaphoreInfos = &imageAcquireWaitInfo, 
		.commandBufferInfoCount = 1,
		.pCommandBufferInfos = &cmdSubmitInfo,
		.signalSemaphoreInfoCount = static_cast<uint32_t>(semaphoreSignals.size()),
		.pSignalSemaphoreInfos = semaphoreSignals.data()
	};

    const queue_data* gfx = m_vkContext.get_device().get_graphics_queue();
    vkQueueSubmit2(gfx->s_handle, 1, &submitInfo, VK_NULL_HANDLE);


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


	vkQueuePresentKHR(gfx->s_handle, &presentInfo);

}

void VkRenderer::clean_swapchain_v2()
{
    swapchain.get()->Destroy();
}

void VkRenderer::recreate_swapchain_v2()
{

}



//------------------------------------[RENDER HELPER FUNCTION]---------------------------------//



