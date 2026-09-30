#pragma once

#include "vulkan/vulkan.h"
#include "Renderer.hpp"
#include "VkContext.hpp"

#include <vector>
#include <array>
#include <iostream>

#include "VkContext.hpp"
#include "VkContexts/VkSwapChainContext.hpp"
#include "VkConstants.hpp"


#include "VulkanSwapchain.hpp"



#include "Resources/Buffers.hpp"
#include "Resources/Buffer.hpp"



#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "ShaderOBJ.hpp"

#include "Sync/VkSynchronization.hpp"


#include "VkDescHeapMng.hpp"


#include <cstdlib>

class VkRenderer : public Renderer
{
public:
    VkRenderer(VkContext& ctx) : Renderer(ctx), m_vkContext(ctx), m_heap_manager(){}

    void Initialize(Context&)                override;
    void RenderFrame()                       override;
    void Shutdown()                          override;
    void UpdateResolution(const Resolution&) override;

    void create_swapchain();


    void create_frame_data_v2();
    void create_sync_resources();
    void clean_new_sync();
    void render_with_shader_objects();

    void clean_swapchain_v2();
    void recreate_swapchain_v2();

    //just for now its sets all states no costomization
    void set_all_states(VkCommandBuffer&);


    void record_to_buffer(VkCommandBuffer&);

private:
    VkContext&                        m_vkContext;
    std::unique_ptr<VulkanSwapchain>  swapchain;
    VkSwapchainContext                swapchainContext;
    
    std::unique_ptr<ShaderOBJ::Shader<ShaderType::VERTEX> >   vertex_obj;
    std::unique_ptr<ShaderOBJ::Shader<ShaderType::FRAGMENT> > fragment_obj;
    std::unique_ptr<
        ShaderOBJ::ShaderSuite<
            ShaderType::VERTEX,
            ShaderType::FRAGMENT
        >
    > suite;

    bool m_framebufferResized = false;


    VkDescHeapMng m_heap_manager;

    BufferPtr m_vert_buff_ptr;
    buffer vertex_buffer;
    std::vector<DVS::VertexBuffer> input_vertex_buffers;

    buffer index_buffer;
    const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0
    };
    void construct_vertex_buffer();

    std::vector<buffer> uniform_buffers;

    void update_uniform_buffer(uint32_t currentImage);


    UBO_TEST ubo{};
    BufferPtr m_uniform_buff_ptr;
    buffer ubo_buff;

private:
    //------------------------------------[SYNCHRONIZATION DATA]---------------------------------//
    struct frameData
    {
        VkCommandPool   s_commandPool     = nullptr;
        VkCommandBuffer s_commandBuffer   = nullptr;
        VkSemaphore     s_imgAcquiredSmph = nullptr;
    };
    std::array<frameData, MAX_FRAMES_IN_FLIGHT> m_frameResources;

    uint32_t currentFrame    = 0;
    uint64_t nextSignalValue = MAX_FRAMES_IN_FLIGHT + 1;


    VkSemaphore timelineSmph = nullptr; // cpu-gpu
    std::vector<VkSemaphore> m_renderCompleteSmphs;//should move to swapchain
    
    //------------------------------------------------------------------------------------------//


    bool m_swapchainRecreation = false;

    VkImage depthImage = nullptr;
    VkImageView depthImageView = nullptr;
private: //policies

using present_policy = vk_sync::ImageMem_2
    <vk_sync::Stage::ColorAttachment,vk_sync::Action::ColorWrite,
    vk_sync::Stage::None, vk_sync::Action::Zero>;


};
