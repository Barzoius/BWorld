#pragma once

#include "vulkan/vulkan.h"

#include "VertexSystem.hpp"

#include "VMA/vk_mem_alloc.h"

#include "Queue.hpp"

#include "Systems/System.hpp"

struct buffer
{
    VkBuffer s_handle = VK_NULL_HANDLE;
    VmaAllocation s_allocation = nullptr;
    VkDeviceSize s_size = 0;
};

struct index_buffer_data
{
    VkBuffer s_handle;
    VmaAllocation s_allocation;
};


struct vertex_buffer_data
{
    VkBuffer s_handle;
    VmaAllocation s_allocation;
    uint32_t s_size;
};

struct uniform_buffer_data
{

};


buffer create_vertex_buffer(DVS::VertexBuffer&, VmaAllocator);
buffer create_vertex_buffer_with_staging(System&, DVS::VertexBuffer&, VmaAllocator);
buffer create_index_buffer(System&, const std::vector<uint16_t>&, VmaAllocator);
void create_uniform_buffer();

void delete_buffer(buffer&, VmaAllocator allocator);


buffer create_buffer(VmaAllocator allocator,
                     VkDeviceSize size,
                     VkBufferUsageFlags usage,
                     VmaMemoryUsage memoryUsage,
                     VmaAllocationCreateFlags flags = 0 );

void upload_to_buffer(VmaAllocator allocator,
                      const buffer& buffer,
                      const void* data,
                      VkDeviceSize size  );

