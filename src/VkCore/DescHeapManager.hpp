#pragma once
#include "vulkan/vulkan.h"

#include <array>

#include "VkConstants.hpp"
#include "VkContext.hpp"
#include "VkUtils.hpp"
#include "Resources/Buffers.hpp"

class DescHeapManager
{
public:
    DescHeapManager() = default;

    void init(const VkPhysicalDevice& dev)
    {
        m_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_HEAP_PROPERTIES_EXT;
        VkPhysicalDeviceProperties2 device_props_2
        {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
            .pNext = &m_properties
        };
	    vkGetPhysicalDeviceProperties2(dev, &device_props_2);

        size_t resource_count = DESC_LIMIT + MAX_FRAMES_IN_FLIGHT * PER_FRAME_DESC_LIMIT;
       
        m_sizes.buffer_descriptor_size 
        = vkutil::aligned_size(m_properties.bufferDescriptorSize, m_properties.bufferDescriptorAlignment);
        
        m_sizes.image_heap_offset 
        = vkutil::aligned_size(m_sizes.buffer_descriptor_size * resource_count, m_properties.imageDescriptorAlignment);

        m_sizes.image_descriptor_size 
        = vkutil::aligned_size(m_properties.imageDescriptorSize, m_properties.imageDescriptorAlignment);

        m_sizes.sampler_descriptor_size 
        = vkutil::aligned_size(m_properties.samplerDescriptorSize, m_properties.samplerDescriptorAlignment);

        m_sizes.heap_buffer_size 
        = vkutil::aligned_size(m_sizes.image_heap_offset + resource_count * m_sizes.image_descriptor_size +
        m_properties.minResourceHeapReservedRange, m_properties.resourceHeapAlignment);

        m_sizes.heap_sampler_size
        = vkutil::aligned_size(SAMPLER_DESC_LIMIT + m_sizes.sampler_descriptor_size + m_properties.minSamplerHeapReservedRange, m_properties.samplerDescriptorAlignment);
    }



    void create_buffer_heap(VkDevice dev, VmaAllocator alloc)
    {
        m_descHeapResources = create_buffer(alloc, m_sizes.heap_buffer_size, VK_BUFFER_USAGE_DESCRIPTOR_HEAP_BIT_EXT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, VMA_ALLOCATION_CREATE_MAPPED_BIT);

        VkBufferDeviceAddressInfo addressInfo{
            .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
            .buffer = m_descHeapResources.s_handle
        };

        m_descHeapResources.s_address = vkGetBufferDeviceAddress(dev, &addressInfo);

      
        
    }


    void upload_buffer(VulkanDevice dev, buffer buf)
    {

        VkBufferDeviceAddressInfo addressInfo{
            .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
            .buffer = buf.s_handle
        };

        buf.s_address = vkGetBufferDeviceAddress(dev.get(), &addressInfo);

        VkDeviceAddressRangeEXT buffer_device_addr_range
        {
            .address = buf.s_address,
            .size    = buf.s_size
        };

        VkResourceDescriptorInfoEXT buffer_resource_desc
        {
            .sType = VK_STRUCTURE_TYPE_RESOURCE_DESCRIPTOR_INFO_EXT,
            .type  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .data  = {.pAddressRange = &buffer_device_addr_range}
        };

        VkHostAddressRangeEXT buffer_host_address
        {
            .address = (uint8_t *) (m_descHeapResources.s_data), 
            .size    = m_sizes.buffer_descriptor_size
        };

        dev.vkWriteResourceDescriptorsEXT(dev.get(), 1, &buffer_resource_desc, &buffer_host_address);

    }

    void create_sampler_heap()
    {
        std::array<VkHostAddressRangeEXT, 2> host_address_ranges_samplers{};
    }


    void create_heaps()
    {

    }


private:
public:
    VkPhysicalDeviceDescriptorHeapPropertiesEXT m_properties{};
    struct Sizes
    {
        VkDeviceSize buffer_heap_offset{0};
        VkDeviceSize buffer_descriptor_size{0};

        VkDeviceSize image_heap_offset{0};
        VkDeviceSize image_descriptor_size{0};

        VkDeviceSize sampler_heap_offset{0};
        VkDeviceSize sampler_descriptor_size{0};

        VkDeviceSize heap_buffer_size{0};
        VkDeviceSize heap_sampler_size{0};
    }                                            m_sizes;

public:
    buffer m_descHeapResources;
    buffer m_descHeapSamplers;
};  