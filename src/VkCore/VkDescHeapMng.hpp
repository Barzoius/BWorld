#pragma once

#include "Resources/Buffer.hpp"
#include "Resources/Image.hpp" 
#include "Resources/Sampler.hpp"


class VulkanDevice;
class VkDescHeapMng
{
public:
    VkDescHeapMng() = default;

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



    void create_buffer_heap(VulkanDevice& dev)
    {

        BufferConfig resource_heap_config(m_sizes.heap_buffer_size);
        resource_heap_config.with_usage(VK_BUFFER_USAGE_DESCRIPTOR_HEAP_BIT_EXT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
        .with_vma_usage(VMA_MEMORY_USAGE_CPU_TO_GPU)
        .with_vma_flags(VMA_ALLOCATION_CREATE_MAPPED_BIT);


        m_descHeapResources = resource_heap_config.build_ptr(dev);     
    }

    void create_sampler_heap(VulkanDevice& dev)
    {
        BufferConfig resource_heap_config(m_sizes.heap_buffer_size);
        resource_heap_config.with_usage(VK_BUFFER_USAGE_DESCRIPTOR_HEAP_BIT_EXT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
        .with_vma_usage(VMA_MEMORY_USAGE_CPU_TO_GPU)
        .with_vma_flags(VMA_ALLOCATION_CREATE_MAPPED_BIT);


        m_descHeapSamplers = resource_heap_config.build_ptr(dev);
    }



    uint32_t upload_buffer(VulkanDevice& dev, BufferPtr& buf)
    {
        const uint32_t size = static_cast<uint32_t>(m_sizes.buffer_descriptor_size);
        const uint32_t offset = m_nextBufferOffset;

        const VkDeviceSize limit = m_sizes.image_heap_offset;

        if (offset + size > limit)
            throw std::runtime_error("Descriptor heap buffer region exhausted");

        m_nextBufferOffset += size;

        VkDeviceAddressRangeEXT buffer_device_addr_range
        {
            .address = buf->get_device_address(),
            .size    = buf->get_size()
        };

        VkResourceDescriptorInfoEXT buffer_resource_desc
        {
            .sType = VK_STRUCTURE_TYPE_RESOURCE_DESCRIPTOR_INFO_EXT,
            .type  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .data  = {.pAddressRange = &buffer_device_addr_range}
        };



        VkHostAddressRangeEXT buffer_host_address
        {
            .address = (uint8_t *) (m_descHeapResources->get_data()) + offset, // pay attention here
            .size    = m_sizes.buffer_descriptor_size
        };



        dev.vkWriteResourceDescriptorsEXT(
            dev.get(), 
            1, 
            &buffer_resource_desc, 
            &buffer_host_address
        );

        //std::cout<<"OFFSET: "<<m_nextBufferOffset<<"\n";
        return offset;

    }

    void upload_image(VulkanDevice& dev)
    {

    }

    void upload_sampler(VulkanDevice& dev, Sampler sampler)
    {
        // we dont actaully need a created Smapler just the create sampler info
    }
    
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
    } m_sizes;

public:
    
    BufferPtr m_descHeapResources;
    BufferPtr m_descHeapSamplers;

    uint32_t m_nextBufferOffset  = 0;
    uint32_t m_nextSamplerOffset = 0;


};