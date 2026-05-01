#pragma once 

#include  "vulkan/vulkan.h"
#include "VkContext.hpp"
#include "VkUtils.hpp"
#include "VkContexts/VkSwapChainContext.hpp"
#include "Pipeline/RenderPass.hpp"

class VulkanCommandPool
{
public:
    VulkanCommandPool(VkContext&, VkSwapchainContext&, RenderPass&, std::vector<VkFramebuffer>&);
    ~VulkanCommandPool();
    VulkanCommandPool(const VkCommandPool&) = delete;
    VulkanCommandPool& operator=(const VkCommandPool) = delete;

    void Initialize();
    void Destroy();

    void initBuffer();
    void recordCommandBuffer(uint32_t imageIndex); 
    
    VkCommandPool get_handle() const;

private:
    VkContext& context;
    VkSwapchainContext swapchainContext;
    RenderPass& renderPass;
    std::vector<VkFramebuffer> swapchainFrameBuffers;
    VkCommandPool handle{};

    VkCommandBuffer commandBuffer{};
};