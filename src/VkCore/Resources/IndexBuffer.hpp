#pragma once
#include "VkContext.hpp"


class IndexBuffer 
{
public:
    IndexBuffer(VkContext&, const std::vector<uint16_t>);
    ~IndexBuffer();

    void create_buffer();
    void destroy_buffer();
    void alloc_and_bind_mem();

    size_t get_count();
    VkBuffer get_handle();

private:
    VkContext& m_context;

    const std::vector<uint16_t> m_indices;

    VkBuffer m_vkBuffer;
    VkDeviceMemory m_vkMem;
};