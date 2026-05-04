#pragma once

#include "vulkan/vulkan.h"
#include "Renderer.hpp"
#include "VkContext.hpp"

#include <vector>
#include <iostream>

#include "VkContext.hpp"
#include "Pipeline/GraphicsPipeline.hpp"

#include "Pipeline/RenderPass.hpp"

#include "VulkanSwapchain.hpp"

#include "VulkanCommandPool.hpp"

class VkRenderer : public Renderer
{
public:
    VkRenderer(VkContext& ctx) : Renderer(ctx), vkContext(ctx){}
    void Initialize(Context&) override;
    void RenderFrame() override;
    void Shutdown() override;

    void UpdateResolution(const Resolution&) override;

    void create_swapchain();
    void create_renderpass();
    void create_GFX_pipeline();
    void create_commandpool();
    void recreate_swapcahin();

    // things to move from here
    void create_framebuffers();

private:
    VkContext& vkContext;
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

    /// everything here gets moved
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;

    VkCommandBuffer commandBuffer{};

    void createSyncObjects();
    void createCommandBuffer();
    void recordCommandBuffer(uint32_t imageIndex);
};
