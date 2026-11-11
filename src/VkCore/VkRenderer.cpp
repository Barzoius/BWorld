#include "VkRenderer.hpp"
#include <memory>



#include "VertexSystem.hpp"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>

#include "UniformSystem.hpp"


#include "Resources/Buffer.hpp"
#include "Resources/Image.hpp"



#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.hpp"

#include <filesystem>
namespace fs = std::filesystem;

void load()
{
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load("C:/Users/mecca/Desktop/MASTER_1/BWorld/src/Media/Image/Images/brickwall.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }
}

void VkRenderer::Initialize(Context& context) 
{

    alloc::initialize_vma_allocator(m_vkContext);

    m_heap_manager.init(m_vkContext.get_device().getPhyD());
    m_heap_manager.create_buffer_heap(m_vkContext.get_device());
    

    vertex_obj = std::make_unique<ShaderOBJ::Shader<ShaderType::VERTEX>>(
        m_vkContext.get_device().get(), 
        VK_SHADER_STAGE_FRAGMENT_BIT, 
        "vertex_1", 
        "Shaders/base3.vert.spv");

    fragment_obj = std::make_unique<ShaderOBJ::Shader<ShaderType::FRAGMENT>>(
        m_vkContext.get_device().get(), 
        0, \
        "fragment_1", 
        "Shaders/base1.frag.spv");  

    VkDescriptorSetAndBindingMappingEXT buff_bind{};
    buff_bind.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_AND_BINDING_MAPPING_EXT;
    buff_bind.pNext = nullptr;
    buff_bind.descriptorSet = 0;
    buff_bind.firstBinding = 0;
    buff_bind.bindingCount = 1;
    buff_bind.resourceMask = VK_SPIRV_RESOURCE_TYPE_UNIFORM_BUFFER_BIT_EXT;
    buff_bind.source = VK_DESCRIPTOR_MAPPING_SOURCE_HEAP_WITH_CONSTANT_OFFSET_EXT;
    


    buff_bind.sourceData.constantOffset.heapArrayStride =
        static_cast<uint32_t>(
            m_heap_manager.m_sizes.buffer_descriptor_size
        );

    VkShaderDescriptorSetAndBindingMappingInfoEXT mapping_info{};
    mapping_info.sType =
        VK_STRUCTURE_TYPE_SHADER_DESCRIPTOR_SET_AND_BINDING_MAPPING_INFO_EXT;
    mapping_info.pNext = nullptr;
    mapping_info.mappingCount = 1;
    mapping_info.pMappings = &buff_bind;

    vertex_obj->set_pNext(mapping_info);
    fragment_obj->set_pNext(mapping_info);

    suite = std::make_unique<ShaderOBJ::ShaderSuite<
            ShaderType::VERTEX,
            ShaderType::FRAGMENT
        >>(m_vkContext.get_device(), *vertex_obj, *fragment_obj);
    suite->link();

 
   
    create_swapchain();
    


  
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), swapchain -> get_width() / (float) swapchain -> get_height(), 0.1f, 10.0f);
    //ubo.proj[1][1] *= -1.0f;


    BufferConfig ubo_config(sizeof(ubo));
    ubo_config
    .with_debug_name("uniform_buff_1")
    .with_usage(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | 
    VK_BUFFER_USAGE_2_SHADER_DEVICE_ADDRESS_BIT)
    .with_vma_flags(VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT).
    with_sharing_mode(VK_SHARING_MODE_EXCLUSIVE);

    m_uniform_buff_ptr = ubo_config.build_ptr(m_vkContext.get_device());
    m_uniform_buff_ptr->update(ubo);

    
    m_heap_manager.upload_buffer(m_vkContext.get_device(), m_uniform_buff_ptr);



    create_sync_resources();

    create_frame_data();

}

void VkRenderer::Submit(const RenderObject& obj)
{
    m_render_objects.emplace_back(obj);
    GPU_Mesh mesh;

    //-----VERTEX BUFFER-----//
    {
        const auto& vb = obj.mesh->m_vertices;

        BufferConfig stag_config(vb.get_size_in_bytes());
        stag_config
        .with_debug_name("stage_buff_1")
        .with_vma_flags(VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT)
        .with_usage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

        Buffer stag_buffer(m_vkContext.get_device(), stag_config);


        const auto data = vb.get_data();
        const auto size = vb.get_size_in_bytes();

        if(vb.get_data() != nullptr) 
            stag_buffer.update(vb.get_data(), size);



        BufferConfig config(vb.get_size_in_bytes());
        config.with_debug_name("vert_buff_1")
        .with_usage(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
        .with_vma_usage(VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);
        
        mesh.vertex_buffer = config.build_ptr(m_vkContext.get_device()); 

        copy_buffer(m_vkContext.transfer_sys, stag_buffer.get_handle(), 
        mesh.vertex_buffer.get()->get_handle(), vb.get_size_in_bytes());


        // stag_buffer.destroy();
    }

    //-----INDEX BUFFER-----//
    {
        const auto& indices = obj.mesh->m_indices;

        mesh.index_count = static_cast<uint32_t>(indices.size());

        size_t buffSize = sizeof(indices[0]) * indices.size();

        BufferConfig stag_config_2(buffSize);
        stag_config_2
        .with_debug_name("stage_buff_2")
        .with_vma_flags(VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT)
        .with_usage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

        Buffer stag_buffer_2(m_vkContext.get_device(), stag_config_2);

        
        stag_buffer_2.update(indices);

        BufferConfig config_index(buffSize);
        config_index.with_debug_name("index_buff_1")
        .with_usage(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
        .with_vma_usage(VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);
        
        mesh.index_buffer = config_index.build_ptr(m_vkContext.get_device());

        copy_buffer(m_vkContext.transfer_sys, stag_buffer_2.get_handle(), 
        mesh.index_buffer.get()->get_handle(), buffSize);

        // stag_buffer_2.destroy();
    }

    m_meshes.emplace_back(std::move(mesh));

}




void VkRenderer::RenderFrame() 
{
    render();
}



void VkRenderer::Shutdown() {
    
    vkDeviceWaitIdle(m_vkContext.get_device().get());


    clean_new_sync();


    for (VkSemaphore &semaphore : m_renderCompleteSmphs)
	{
		vkDestroySemaphore(m_vkContext.get_device().get(), semaphore, nullptr);
	}
	m_renderCompleteSmphs.clear();


    //m_test_image->destroy();

    m_render_objects.clear();
    m_meshes.clear();


    m_uniform_buff_ptr.get()->destroy();

    m_heap_manager.m_descHeapResources->destroy();



    alloc::destroy_vma_allocator();


    clean_swapchain_v2();

    if(suite)
    {
        suite->destroy();	
    }
    std::cout<<"RENDER SHUTDOWN END\n";

}

 void VkRenderer::UpdateResolution(const Resolution& res)
 {
    swapchain.get()->update_resolution(res.width, res.height);
    
    m_vkContext.update_instance_resolution(res);

    LLOGI("Resolution updated: %d x %d", res.width, res.height);
 }



void VkRenderer::create_swapchain()
{

    swapchain = std::make_unique<VulkanSwapchain>(m_vkContext.get_device(), 
     m_vkContext.get_instance().get_surface_handle(), 
                                                         m_vkContext.get_instance().get_resolution().width, 
                                                         m_vkContext.get_instance().get_resolution().height);
    swapchain -> createSwapChain();
    swapchain -> create_swapchain_image_views();


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

    VK_ASSERT_MSG(vkCreateSemaphore(m_vkContext.get_device().get(), &smphInfo, nullptr, &timelineSmph), "Failed to create timeline semaphore!");


    m_renderCompleteSmphs.resize(swapchain.get()->get_images().size());
    for(VkSemaphore& smph : m_renderCompleteSmphs)
    {
        VkSemaphoreCreateInfo smphInfo{ .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
        VK_ASSERT_MSG(vkCreateSemaphore(m_vkContext.get_device().get(), &smphInfo, nullptr, &smph), "failed to create semaphore for render completion!");
    }
}



void VkRenderer::clean_new_sync()
{
    if(timelineSmph)
        vkDestroySemaphore(m_vkContext.get_device().get(), timelineSmph, nullptr);
    

    clean_frame_data();
}



void VkRenderer::create_frame_data()
{
    m_frameResources2.reserve(MAX_FRAMES_IN_FLIGHT);
    for(int i = 0 ; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        m_frameResources2.emplace_back(m_vkContext.get_device());

        VkSemaphoreCreateInfo smphInfo
        {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
        };
        
        VK_ASSERT_MSG(
        vkCreateSemaphore(m_vkContext.get_device().get(), 
                        &smphInfo, nullptr, 
                        &m_frameResources2.back().s_imgAcquiredSmph), 
                        "failed to create semaphore for image acquisition!");
    }
}

void VkRenderer::clean_frame_data()
{
    for(frame_data &data : m_frameResources2)
    {
        vkDestroySemaphore(m_vkContext.get_device().get(), data.s_imgAcquiredSmph, nullptr);
        m_vkContext.get_device().destroy_cmd_pool(data.s_pool.get_handle());
    }
}


void VkRenderer::create_camera_ubo()
{
    m_cam_data.view = glm::lookAt(
        glm::vec3(2.0f, 2.0f, 2.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );
    m_cam_data.proj = glm::perspective(
        glm::radians(45.0f),
        swapchain -> get_width() / (float)(swapchain -> get_height()),
        0.1f,
        10.0f
    );
    m_cam_data.proj[1][1] *= -1.0f;

    BufferConfig cameraConfig(sizeof(CameraUBO));
    cameraConfig
    .with_debug_name("camera_ubo")
    .with_usage(
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT |
        VK_BUFFER_USAGE_2_SHADER_DEVICE_ADDRESS_BIT
    )
    .with_vma_flags(
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
        VMA_ALLOCATION_CREATE_MAPPED_BIT
    )
    .with_sharing_mode(VK_SHARING_MODE_EXCLUSIVE);

    m_camera_ubo = cameraConfig.build_ptr(m_vkContext.get_device());

    m_camera_ubo->update(m_cam_data);


}


void VkRenderer::render()
{
    if(m_swapchainRecreation)
    {
        vkDeviceWaitIdle(m_vkContext.get_device().get());
        clean_swapchain_v2();
        create_swapchain();
        m_swapchainRecreation = false;
    }

///=================

    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    

    m_uniform_buff_ptr->update(ubo);

    const uint32_t frameDataIndex = currentFrame++ % MAX_FRAMES_IN_FLIGHT;
    const uint64_t signalValue    = nextSignalValue++;
    const uint64_t waitValue      = signalValue - MAX_FRAMES_IN_FLIGHT;

    VkSemaphoreWaitInfo waitInfo
    {
        .sType          = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
        .semaphoreCount = 1,
        .pSemaphores    = &timelineSmph,
        .pValues        = &waitValue
    };
    vkWaitSemaphores(m_vkContext.get_device().get(), &waitInfo, UINT64_MAX);

    frame_data &data = m_frameResources2[frameDataIndex]; 
    data.s_pool.reset();

    VkSemaphore imgAquireSemph = data.s_imgAcquiredSmph;

    
    uint32_t imageIndex = 0;

    /// move to swapchain -----------------------------
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
    //// --------------------------------------------------

    
    
    data.s_buffer.begin();


    std::vector<VkImageMemoryBarrier2> layoutBarriers
	{
		{
			.sType         = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
			.srcStageMask  = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
			.srcAccessMask = 0,
			.dstStageMask  = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
			.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
			.oldLayout     = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.image         = swapchain.get() -> get_images()[imageIndex],
			.subresourceRange
			{
				.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel   = 0,
				.levelCount     = 1,
				.baseArrayLayer = 0,
				.layerCount     = 1,
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

    vkCmdPipelineBarrier2(data.s_buffer.get_handle(), &depInfo);

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
            .extent{.width = static_cast<uint32_t>(swapchain -> get_width()), .height = static_cast<uint32_t>(swapchain -> get_height())}
        },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttInfo,
        .pDepthAttachment = &depthAttInfo
    };




data.s_buffer.begin_render(renderInfo);
{

    data.s_buffer.set_vertex_input(DVS::get_universal_layout());

    // Set states
    set_all_states(data.s_buffer.get_handle());

    // --------------------------------------------------
    // Draw
    // --------------------------------------------------

    VkBindHeapInfoEXT bind_heap_info_res{
        .sType = VK_STRUCTURE_TYPE_BIND_HEAP_INFO_EXT,
        .heapRange{
            .address = m_heap_manager.m_descHeapResources->get_device_address(),
            .size    = m_heap_manager.m_descHeapResources->get_size()},
        .reservedRangeOffset = m_heap_manager.m_descHeapResources->get_size() - m_heap_manager.m_properties.minResourceHeapReservedRange,
        .reservedRangeSize = m_heap_manager.m_properties.minResourceHeapReservedRange,
    };

    
    m_vkContext.get_device().vkCmdBindResourceHeapEXT(data.s_buffer.get_handle(), &bind_heap_info_res);

    suite -> bind(data.s_buffer.get_handle());

    data.s_buffer.bind_vertex_buffers(m_meshes[1].vertex_buffer, 0);
    data.s_buffer.bind_index_buffers(m_meshes[1].index_buffer);

    vkCmdDrawIndexed(
        data.s_buffer.get_handle(),
        m_meshes[1].index_count,
        1,
        0,
        0,
        0
    );
}
data.s_buffer.end_render();  


    vk_sync::Barrier<present_policy> present_barrier;
    present_barrier.set_old_layout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    present_barrier.set_new_layout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    present_barrier.set_image(swapchain.get() -> get_images()[imageIndex]);
    VkImageSubresourceRange subs{
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,       
    };
    present_barrier.set_subresource_range(subs);
    present_barrier.bind_barrier(data.s_buffer.get_handle());



    data.s_buffer.end();
    ////////////////////////////////////////////////////////////////
    // END COMMAND BUFFER
    ////////////////////////////////////////////////////////////////

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
		.commandBuffer = data.s_buffer.get_handle(),
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

    VkSwapchainPresentFenceInfoKHR vk;
    
	vkQueuePresentKHR(gfx->s_handle, &presentInfo);
}



void VkRenderer::clean_swapchain_v2()
{
    swapchain.get()->destroy();
}



void VkRenderer::set_all_states(VkCommandBuffer cmd)
{
    ///------set states------///

    VkViewport viewport{
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(swapchain -> get_width()),
        .height = static_cast<float>(swapchain -> get_height()),
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };


    VkRect2D scissor{
        .offset = {0, 0},
        .extent = {
            static_cast<uint32_t>(swapchain -> get_width()),
            static_cast<uint32_t>(swapchain -> get_height())
        }
    };


    m_vkContext.get_device().vkCmdSetViewportWithCountEXT(cmd, 1, &viewport);
    m_vkContext.get_device().vkCmdSetScissorWithCountEXT(cmd, 1, &scissor);


	m_vkContext.get_device().vkCmdSetCullModeEXT(cmd, VK_CULL_MODE_BACK_BIT);
	m_vkContext.get_device().vkCmdSetFrontFaceEXT(cmd,  VK_FRONT_FACE_CLOCKWISE);

    m_vkContext.get_device().vkCmdSetDepthTestEnableEXT(cmd, VK_FALSE);
	m_vkContext.get_device().vkCmdSetDepthWriteEnableEXT(cmd, VK_FALSE);
	m_vkContext.get_device().vkCmdSetDepthCompareOpEXT(cmd, VK_COMPARE_OP_LESS);

    m_vkContext.get_device().vkCmdSetPrimitiveTopologyEXT(cmd, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    m_vkContext.get_device().vkCmdSetRasterizerDiscardEnableEXT(cmd, VK_FALSE);
	m_vkContext.get_device().vkCmdSetPolygonModeEXT(cmd, VK_POLYGON_MODE_FILL);
	m_vkContext.get_device().vkCmdSetRasterizationSamplesEXT(cmd, VK_SAMPLE_COUNT_1_BIT);
	m_vkContext.get_device().vkCmdSetAlphaToCoverageEnableEXT(cmd, VK_FALSE);
	m_vkContext.get_device().vkCmdSetDepthBiasEnableEXT(cmd, VK_FALSE);
	m_vkContext.get_device().vkCmdSetStencilTestEnableEXT(cmd, VK_FALSE);
	m_vkContext.get_device().vkCmdSetPrimitiveRestartEnableEXT(cmd, VK_FALSE);

    const uint32_t sampleMask = 0xFF;
	m_vkContext.get_device().vkCmdSetSampleMaskEXT(cmd, VK_SAMPLE_COUNT_1_BIT, &sampleMask);

	const VkBool32 colorBlendEnables = false;
	const VkColorComponentFlags colorBlendComponentFlags = 0xf;
	const VkColorBlendEquationEXT colorBlendEquation{};
	m_vkContext.get_device().vkCmdSetColorBlendEnableEXT(cmd, 0, 1, &colorBlendEnables);
	m_vkContext.get_device().vkCmdSetColorWriteMaskEXT(cmd, 0, 1, &colorBlendComponentFlags);
}





void image()
{
    {
    //image
    
    // int texWidth, texHeight, texChannels;
    // stbi_uc* pixels = stbi_load("C:/Users/mecca/Desktop/MASTER_1/BWorld/src/Media/Image/Images/brickwall.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    // VkDeviceSize imageSize = texWidth * texHeight * 4;

    // if (!pixels) {
    //     throw std::runtime_error("failed to load texture image!");
    // }

    // BufferConfig stage_img_conf(imageSize);
    // stage_img_conf
    // .with_debug_name("stage_image_config")
    // .with_vma_flags(VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT)
    // .with_usage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

    // Buffer stag_img(m_vkContext.get_device(), stage_img_conf);

    // if(pixels != nullptr) stag_img.update(pixels, imageSize);

    // stbi_image_free(pixels);

    // ImageConfig img_conf(texWidth, texHeight);
    // img_conf.with_debug_name("img_1")
    // .with_usage(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
    // .with_vma_usage(VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);
    
    // m_test_image = img_conf.build_ptr(m_vkContext.get_device());



    // trasition_img_layout(m_vkContext.transfer_sys, m_test_image->get_handle(), VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    // copy_buffer_to_img(m_vkContext.transfer_sys, stag_img.get_handle(),
    //  m_test_image->get_handle(),
    //  static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

    // stag_img.destroy();
    
    }
}