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

class VkRenderer : public Renderer
{
public:
    VkRenderer(VkContext& ctx) : Renderer(ctx), m_vkContext(ctx){}
    void Initialize(Context&) override;
    void RenderFrame() override;
    void Shutdown() override;

    void UpdateResolution(const Resolution&) override;

    void create_swapchain();
    void create_renderpass();
    void create_GFX_pipeline();
    void create_commandpool();

    void clean_swapchain();
    void recreate_swapcahin();

    void create_frame_data();
    // things to move from here
    void create_framebuffers();

private:
    VkContext& m_vkContext;
    std::unique_ptr<VulkanSwapchain> swapchain;
    std::unique_ptr<RenderPass> renderPass;
    std::vector<VkFramebuffer> swapChainFramebuffers;

    VkSwapchainContext swapchainContext;

    std::unique_ptr<GraphicsPipeline> gfxPipeline;

    std::unique_ptr<VulkanCommandPool> commandPool;
    
    std::vector<std::unique_ptr<VulkanPipeline>> gfxPipelines;
    std::vector<std::unique_ptr<VulkanPipeline>> computePipelines;
    std::unique_ptr<Shader<ShaderType::VERTEX>> vertex;
    std::unique_ptr<Shader<ShaderType::FRAGMENT>> fragment;


    void recordCommandBuffer(VkCommandBuffer&, uint32_t);

    bool m_framebufferResized = false;

private:
    static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;
    uint32_t currentFrame = 0;


    std::vector<VkCommandBuffer> m_commandBuffer;
    std::vector<VkSemaphore> m_imgAvailableSmph;
    std::vector<VkSemaphore> m_renderFinishedSmph;
    std::vector<VkFence> m_inFlightFence;

    uint64_t frameValue; 


    VkSemaphore m_gpuTimeline;
    uint64_t m_gpuCounter = 0;
    void create_gpu_timeline();

};
