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

class VkRenderer : public Renderer
{
public:
    VkRenderer(VkContext& ctx) : Renderer(ctx), vkContext(ctx), renderPass(ctx) {}
    void Initialize(Context&) override;
    void RenderFrame() override;
    void Shutdown() override;

    void UpdateResolution(const Resolution&) override;

    void CreateSwapChain();
    void RecreateSwapCahin();

private:
    VkContext& vkContext;
    std::unique_ptr<VulkanSwapchain> swapchain;

    RenderPass renderPass;
    std::vector<std::unique_ptr<VulkanPipeline>> gfxPipelines;
    std::vector<std::unique_ptr<VulkanPipeline>> computePipelines;
    std::unique_ptr<Shader<ShaderType::VERTEX>> vertex;
    std::unique_ptr<Shader<ShaderType::FRAGMENT>> fragment;

    
};
