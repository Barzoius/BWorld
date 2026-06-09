#include "IndexBuffer.hpp"

IndexBuffer::IndexBuffer(VkContext& p_context,const std::vector<uint16_t> p_indices) 
    : m_context(p_context), m_indices(p_indices)
{

}

IndexBuffer::~IndexBuffer() = default;

void IndexBuffer::create_buffer()
{
    VkDeviceSize bufferSize = sizeof(m_indices[0]) * m_indices.size();
    
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
}


void IndexBuffer::alloc_and_bind_mem()
{

}


void IndexBuffer::destroy_buffer()
{

}

VkBuffer IndexBuffer::get_handle()
{
    return m_vkBuffer;
}