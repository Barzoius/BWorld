#pragma once

#include "vulkan/vulkan.h"

#include "VertexSystem.hpp"
#include "UniformSystem.hpp"

#include "VMA/vk_mem_alloc.hpp"

#include "Queue.hpp"

#include "Systems/System.hpp"

#include <glm/glm.hpp>


struct buffer
{
    VkBuffer          s_handle = VK_NULL_HANDLE;
    VmaAllocation     s_allocation = nullptr;
    VkDeviceSize      s_size = 0;
    VmaAllocationInfo s_info;
    VkDeviceAddress   s_address;
    void*             s_data;
};


struct UBO_TEST{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};


VkFormat get_vertex_buffer_format(DVS::VertexLayout::ElementType type) noexcept;

buffer create_vertex_buffer(DVS::VertexBuffer&, VmaAllocator);
buffer create_vertex_buffer_with_staging(System&, DVS::VertexBuffer&, VmaAllocator);
buffer create_index_buffer(System&, const std::vector<uint16_t>&, VmaAllocator);
buffer create_uniform_buffer(UBO_TEST, VmaAllocator);


// template<typename Policy>
// buffer create_uniform_buffer(DUS::Buffer<Policy>& buf, VmaAllocator alloc)
// {
//     VkDeviceSize size = static_cast<uint64_t>(buf.get_size_in_bytes());

//     buffer b = create_perisistent_buffer(alloc, size, 
//     VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO,
//     VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT);

//     b.s_data = buf.get_data();

//     memcpy(b.s_info.pMappedData, buf.get_data(), buf.get_size_in_bytes());

//     return b;
// }

buffer create_perisistent_buffer(VmaAllocator allocator,
                                VkDeviceSize size,
                                VkBufferUsageFlags usage,
                                VmaMemoryUsage memoryUsage,
                                VmaAllocationCreateFlags flags);
                                


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

