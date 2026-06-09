#pragma once

#include "vulkan/vulkan.h"
#include "Renderer.hpp"
#include "VkContext.hpp"

#include <vector>
#include <array>
#include <iostream>

#include "VkContext.hpp"
#include "Pipeline/GraphicsPipeline.hpp"

#include "Pipeline/RenderPass.hpp"

#include "VulkanSwapchain.hpp"

#include "VulkanCommandPool.hpp"

#include "Resources/VertexBuffer.hpp"

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
    void create_commandpool();

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
    VkContext& m_vkContext;
    std::unique_ptr<VulkanSwapchain> swapchain;
    std::unique_ptr<RenderPass> renderPass;
    std::vector<VkFramebuffer> swapChainFramebuffers;

    VkSwapchainContext swapchainContext;

    std::unique_ptr<GraphicsPipeline> gfxPipeline;

    std::unique_ptr<VulkanCommandPool> commandPool;
    
    std::unique_ptr<Shader<ShaderType::VERTEX>> vertex;
    std::unique_ptr<Shader<ShaderType::FRAGMENT>> fragment;


    void recordCommandBuffer(VkCommandBuffer&, uint32_t);

    bool m_framebufferResized = false;


    std::unique_ptr<VertexBuffer> m_vertexBuffer;

    //to be removed
    void construct_vertex_buffer();

private:
    static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;
    uint32_t currentFrame = 0;


    std::vector<VkCommandBuffer> m_commandBuffer;
    std::vector<VkSemaphore> m_imgAvailableSmph;
    std::vector<VkSemaphore> m_renderFinishedSmph;
    std::vector<VkFence> m_inFlightFence;

    uint64_t frameValue; 

    struct frameData
    {
        VkCommandPool s_commandPool = nullptr;
        VkCommandBuffer s_commandBuffer = nullptr;
        VkSemaphore s_imgAcquiredSmph = nullptr;
    };

    std::array<frameData, MAX_FRAMES_IN_FLIGHT> m_frameResources;
    VkSemaphore timelineSmph = nullptr; // cpu-gpu
    std::vector<VkSemaphore> m_renderCompleteSmphs; 

    uint64_t nextSignalValue = MAX_FRAMES_IN_FLIGHT + 1;

    bool m_swapchainRecreation = false;

    VkImage depthImage = nullptr;
    VkImageView depthImageView = nullptr;

};
