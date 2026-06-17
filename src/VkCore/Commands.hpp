#pragma once

#include "vulkan/vulkan.h"

#include <iostream>
struct pool_data
{
    VkCommandPool s_handle = VK_NULL_HANDLE;
    uint32_t m_queueFamilyIndex;
};


pool_data create_pool(const VkDevice& device, uint32_t family_index)
{
    pool_data pool{};
    
    VkCommandPoolCreateInfo poolInfo
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .queueFamilyIndex = family_index
    };

    if(vkCreateCommandPool(device, &poolInfo, nullptr, &pool.s_handle) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create transfer command pool!");
    }
}

void delete_pool(pool_data& pool)
{
    
}