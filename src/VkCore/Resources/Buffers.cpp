#include "Buffers.hpp"

#include <iostream>


// rename this one 
VkFormat get_vertex_buffer_format(DVS::VertexLayout::ElementType type) noexcept
{

    switch(type)
    {
        case DVS::VertexLayout::ElementType::Position2D:
            return VK_FORMAT_R32G32_SFLOAT;
        case DVS::VertexLayout::ElementType::Position3D:
            return VK_FORMAT_R32G32B32_SFLOAT;
        case DVS::VertexLayout::ElementType::Texture2D:
            return VK_FORMAT_R32G32_SFLOAT;
        case DVS::VertexLayout::ElementType::Normal:
            return VK_FORMAT_R32G32B32_SFLOAT;;
        case DVS::VertexLayout::ElementType::Tangent:
            return VK_FORMAT_R32G32B32_SFLOAT;          
        case DVS::VertexLayout::ElementType::Bitangent:
            return VK_FORMAT_R32G32B32_SFLOAT;
        case DVS::VertexLayout::ElementType::Float3Color:
            return VK_FORMAT_R32G32B32_SFLOAT;
        case DVS::VertexLayout::ElementType::Float4Color:
            return VK_FORMAT_R32G32B32A32_SFLOAT;   
        case DVS::VertexLayout::ElementType::RGBAColor:
            return VK_FORMAT_R8G8B8A8_UNORM;               
    }

                    
    assert("Invalid element format" && false); return VK_FORMAT_UNDEFINED;
    
}

buffer create_vertex_buffer(DVS::VertexBuffer& vbuf, VmaAllocator allocator)
{
    auto buffer = create_buffer(
        allocator,
        vbuf.get_size_in_bytes(),
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VMA_MEMORY_USAGE_AUTO,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

    upload_to_buffer(
        allocator,
        buffer,
        vbuf.get_data(),
        vbuf.get_size_in_bytes());

    return buffer;
}


buffer create_vertex_buffer_with_staging(System& sys, DVS::VertexBuffer& vbuf, VmaAllocator allocator)
{
    auto stage_buffer = create_buffer(
        allocator,
        vbuf.get_size_in_bytes(),
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VMA_MEMORY_USAGE_AUTO,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

    upload_to_buffer(
        allocator,
        stage_buffer,
        vbuf.get_data(),
        vbuf.get_size_in_bytes());

    auto buffer = create_buffer(
        allocator,
        vbuf.get_size_in_bytes(),
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
        0);

    copy_buffer(sys, stage_buffer.s_handle, buffer.s_handle, vbuf.get_size_in_bytes());

    delete_buffer(stage_buffer, allocator);
    //std::cout<<"STAGING\n";
    return buffer;
}

buffer create_index_buffer(System& sys, const std::vector<uint16_t>& indices, VmaAllocator allocator)
{
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    auto stage_buffer = create_buffer(
        allocator,
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VMA_MEMORY_USAGE_AUTO,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

    upload_to_buffer(
        allocator,
        stage_buffer,
        indices.data(),
        bufferSize);

    auto buffer = create_buffer(
        allocator,
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
        0);

    copy_buffer(sys, stage_buffer.s_handle, buffer.s_handle, bufferSize);

    delete_buffer(stage_buffer, allocator);
    //std::cout<<"STAGING\n";
    return buffer;
}

buffer create_buffer(VmaAllocator allocator,
                    VkDeviceSize size,
                    VkBufferUsageFlags usage,
                    VmaMemoryUsage memoryUsage,
                    VmaAllocationCreateFlags flags)
{
    buffer out{};

    VkBufferCreateInfo bufferInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = memoryUsage;
    allocInfo.flags = flags;

    vmaCreateBuffer(
        allocator,
        &bufferInfo,
        &allocInfo,
        &out.s_handle,
        &out.s_allocation,
        &out.s_info);

    out.s_size = size;
    out.s_data = out.s_info.pMappedData;

    
    return out;
}


void upload_to_buffer(  VmaAllocator allocator,
                        const buffer& buffer,
                        const void* data,
                        VkDeviceSize size  )
{
    void* mapped = nullptr;
    vmaMapMemory(allocator, buffer.s_allocation, &mapped);

    std::memcpy(mapped, data, size);

    vmaUnmapMemory(allocator, buffer.s_allocation);
}

buffer create_perisistent_buffer(
    VmaAllocator allocator,
                                VkDeviceSize size,
                                VkBufferUsageFlags usage,
                                VmaMemoryUsage memoryUsage,
                                VmaAllocationCreateFlags flags)
{
    buffer out{};

    VkBufferCreateInfo bufferInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = memoryUsage;
    allocInfo.flags = flags;

    VkResult result = vmaCreateBuffer(
        allocator,
        &bufferInfo,
        &allocInfo,
        &out.s_handle,
        &out.s_allocation,
        &out.s_info);

    if (result != VK_SUCCESS) std::cout<<"failed buffer creation";

    out.s_size = size;
    return out;
}




buffer create_uniform_buffer(UBO_TEST ubo, VmaAllocator alloc)
{
    VkDeviceSize size = static_cast<uint64_t>(sizeof(UBO_TEST));

    buffer b = create_perisistent_buffer(alloc, size, 
    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_2_SHADER_DEVICE_ADDRESS_BIT, VMA_MEMORY_USAGE_AUTO,
    VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT);

    void* data = &ubo;
    b.s_data = data;

    b.s_data = b.s_info.pMappedData;

    memcpy(b.s_data, data, sizeof(UBO_TEST));

    return b;
}



// buffer create_uniform_buffer(UniformBufferObject ubo, VmaAllocator allocator)
// {
//     VkDeviceSize bufferSize = sizeof(UniformBufferObject);
//     auto buffer = create_buffer(
//         allocator,
//         bufferSize,
//         VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
//         VMA_MEMORY_USAGE_AUTO,
//         VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |    
//         VMA_ALLOCATION_CREATE_MAPPED_BIT // check again these flags
//     );
//     upload_to_buffer(allocator, buffer, &ubo, bufferSize);




//     buffer.s_size = bufferSize;
//     return buffer;
// }

void delete_buffer(buffer& buffer, VmaAllocator allocator)
{
    vmaDestroyBuffer(allocator, buffer.s_handle, buffer.s_allocation);
}


