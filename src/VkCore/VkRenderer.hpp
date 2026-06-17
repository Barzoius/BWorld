#pragma once

#include "vulkan/vulkan.h"
#include "Renderer.hpp"
#include "VkContext.hpp"

#include <vector>
#include <array>
#include <iostream>

#include "VkContext.hpp"
#include "Pipeline/GraphicsPipeline.hpp"


#include "VulkanSwapchain.hpp"

#include "VulkanCommandPool.hpp"

#include "Resources/VertexBuffer.hpp"

#include "Resources/Buffers.hpp"

class VkRenderer : public Renderer
{
public:
    VkRenderer(VkContext& ctx) : Renderer(ctx), m_vkContext(ctx){}
    void Initialize(Context&) override;
    void RenderFrame() override;
    void Shutdown() override;

    void UpdateResolution(const Resolution&) override;

    void create_swapchain();
    void create_GFX_pipeline();


    void clean_swapchain();

    // things to move from here

    void create_frame_data_v2();
    void create_sync_resources();
    void clean_new_sync();
    void render_with_new_sync();

    void clean_swapchain_v2();
    void recreate_swapchain_v2();

    void copy_buffer(VkBuffer, VkBuffer, VkDeviceSize);

private:

    //------------------------------------[RENDER HELPER FUNCTIONS]---------------------------------//
    // uint32_t begin_frame_sync();
    // uint32_t acquire_swapchain_image(frameData&);
    // void record_command_buffer(frameData& frame, uint32_t imageIndex);
    //----------------------------------------------------------------------------------------------//


private:
    VkContext& m_vkContext;
    std::unique_ptr<VulkanSwapchain> swapchain;
    std::vector<VkFramebuffer> swapChainFramebuffers;

    VkSwapchainContext swapchainContext;

    std::unique_ptr<GraphicsPipeline> gfxPipeline;
    
    std::unique_ptr<Shader<ShaderType::VERTEX>> vertex;
    std::unique_ptr<Shader<ShaderType::FRAGMENT>> fragment;


    bool m_framebufferResized = false;



    buffer vertex_buffer;
    std::vector<DVS::VertexBuffer> input_vertex_buffers;

    void construct_vertex_buffer();

private:
    //------------------------------------[SYNCHRONIZATION DATA]---------------------------------//

    static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;

    struct frameData
    {
        VkCommandPool s_commandPool = nullptr;
        VkCommandBuffer s_commandBuffer = nullptr;
        VkSemaphore s_imgAcquiredSmph = nullptr;
    };
    std::array<frameData, MAX_FRAMES_IN_FLIGHT> m_frameResources;

    uint32_t currentFrame = 0;
    uint64_t nextSignalValue = MAX_FRAMES_IN_FLIGHT + 1;


    VkSemaphore timelineSmph = nullptr; // cpu-gpu
    std::vector<VkSemaphore> m_renderCompleteSmphs;//should move to swapchain
    
    //------------------------------------------------------------------------------------------//


    bool m_swapchainRecreation = false;

    VkImage depthImage = nullptr;
    VkImageView depthImageView = nullptr;

};
