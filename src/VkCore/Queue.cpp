#include "Queue.hpp"
#include <iostream>

bool can_present(const queue_data& data)
{
    return data.s_flags & VK_QUEUE_GRAPHICS_BIT;
}

bool can_transfer(const queue_data& data)
{
    return data.s_flags & VK_QUEUE_TRANSFER_BIT;
}

bool can_compute(const queue_data& data)
{
    return data.s_flags & VK_QUEUE_COMPUTE_BIT;
}

void submit(const queue_data& data)
{

}

void present(const queue_data& data)
{

}

// void copy_buffer(const queue_data& queue, const VkDevice& device, const VkCommandPool& pool, VkBuffer& src, VkBuffer& dst, VkDeviceSize size)
// {
//     if(!can_transfer(queue)) throw std::runtime_error("queue cant transfer!");

//     VkCommandBufferAllocateInfo allocInfo{};
//     allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//     allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//     allocInfo.commandPool = pool;
//     allocInfo.commandBufferCount = 1;

//     VkCommandBuffer commandBuffer;
//     vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

//     VkCommandBufferBeginInfo beginInfo{};
//     beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//     beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

//     vkBeginCommandBuffer(commandBuffer, &beginInfo);

//     VkBufferCopy copyRegion{};
//     copyRegion.srcOffset = 0; // Optional
//     copyRegion.dstOffset = 0; // Optional
//     copyRegion.size = size;
//     vkCmdCopyBuffer(commandBuffer, src, dst, 1, &copyRegion);

//     vkEndCommandBuffer(commandBuffer);

//     VkSubmitInfo submitInfo{};
//     submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//     submitInfo.commandBufferCount = 1;
//     submitInfo.pCommandBuffers = &commandBuffer;

//     vkQueueSubmit(queue.s_handle, 1, &submitInfo, VK_NULL_HANDLE);
//     vkQueueWaitIdle(queue.s_handle);

//     vkFreeCommandBuffers(device, pool, 1, &commandBuffer);
// }

