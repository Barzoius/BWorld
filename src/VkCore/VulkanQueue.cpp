#include "VulkanQueue.hpp"

VulkanQueue::VulkanQueue(VkDevice device, uint32_t familyIndex, uint32_t count, float* priorities)
{
    float queuePriority = 1.0f;
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = familyIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
}

VkQueue VulkanQueue::getHandle() const
{
    return handle;
}

void VulkanQueue::createQueue()
{
    vkGetDeviceQueue(device, familyIndex, 0, &handle);
}