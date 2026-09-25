#pragma once

#include "vulkan/vulkan.h"
#include "VkLog.hpp"

class DescriptorAllocator
{
public:
    DescriptorAllocator() = default;
    
    void initialize(const VkDevice& dev)
    {
        VkDescriptorPoolSize sizes[] = 
        {
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,          50 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  30 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,          20 },
        };

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 3;
        poolInfo.pPoolSizes    = sizes;
        poolInfo.maxSets       = 100;

        VkDescriptorPool descriptorPool;
        
        VK_ASSERT_MSG(vkCreateDescriptorPool(dev,&poolInfo,nullptr,&descriptorPool),
        "Failed to create base descriptor pool!");

        m_pools.emplace_back(descriptorPool);
    }

    VkDescriptorSet allocate(const VkDevice& dev, VkDescriptorSetLayout layout)
    {
        VkDescriptorSet desc_set;

        VkDescriptorSetAllocateInfo  allocInfo{};
        allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool     = m_pools[0];
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts        = &layout;

        VK_ASSERT_MSG(vkAllocateDescriptorSets(dev, &allocInfo, &desc_set),
        "failed to allocate descriptor set!");

        return desc_set;
    }
    
    std::vector<VkDescriptorSet> allocate_batch(VkDevice& dev, VkDescriptorSetLayout layout, size_t count = 1)
    {

        std::vector<VkDescriptorSetLayout> layouts(count, layout);

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool     = m_pools[0];
        allocInfo.descriptorSetCount = static_cast<uint32_t>(count);
        allocInfo.pSetLayouts        = layouts.data();

        std::vector<VkDescriptorSet> descriptorSets;
        descriptorSets.resize(count);

        VK_ASSERT_MSG(vkAllocateDescriptorSets(dev, &allocInfo, descriptorSets.data()),
        "failed to allocate descriptor set batch!");

        return descriptorSets;
    }

    void destroy(const VkDevice& dev)
    {
        for(auto& pool : m_pools) vkDestroyDescriptorPool(dev, pool, nullptr);
    }

private:
    std::vector<VkDescriptorPool> m_pools;
};