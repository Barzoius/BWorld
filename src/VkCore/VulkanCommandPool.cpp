#include "VulkanCommandPool.hpp"

VulkanCommandPool::VulkanCommandPool(VkContext& c, VkSwapchainContext& sc, RenderPass& r, std::vector<VkFramebuffer>& swf) : context(c), swapchainContext(sc), renderPass(r), swapchainFrameBuffers(swf)
{
    std::cout << "CommandPool constructor\n";

}

void VulkanCommandPool::Initialize()
{
    vkutil::QueueFamilyIndices queueFamilyIndices = context.get_device().getDeviceIndices();

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(context.get_device().get(), &poolInfo, nullptr, &handle) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }

    std::cout << "CommandPool initialize\n";

}

VulkanCommandPool::~VulkanCommandPool()
{
    

}

VkCommandPool VulkanCommandPool::get_handle() const
{
    return handle;
}


void VulkanCommandPool::Destroy()
{
    vkDestroyCommandPool(context.get_device().get(), handle, nullptr);
    std::cout << "CommandPool destroyed\n";
}

void VulkanCommandPool::initBuffer()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = handle;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(context.get_device().get(), &allocInfo, &commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void VulkanCommandPool::recordCommandBuffer(uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass.getRenderPass();
    renderPassInfo.framebuffer = swapchainFrameBuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapchainContext.extent;
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}



