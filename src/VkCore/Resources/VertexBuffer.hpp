#pragma once
#include "VkContext.hpp"

#include <memory>
#include "glm/common.hpp"
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"


#include "VertexSystem.hpp"

#include "vulkan/vulkan.h"

#include <array>


class VertexBuffer
{
public:
    VertexBuffer(VkContext&,  DVS::VertexBuffer);


    void create_buffer();
    void destroy_buffer() noexcept;
    void destroy_staging_buffer() noexcept;

    void check_mem();
    void alloc_and_bind_mem();
    void alloc_and_bind_mem(VkBuffer&, VkDeviceMemory&);

    void map_mem();

    VkBuffer get_handle();
    VkBuffer get_staging_buffer_handle();

    VkVertexInputBindingDescription get_bind_desc();
    std::vector<VkVertexInputAttributeDescription> get_attr_desc();

    static std::shared_ptr<VertexBuffer> resolve(const std::string&);

    template<typename...Ignore>
    static std::string generate_uid(const std::string&, Ignore&&...ignore);

    size_t get_size();
    
    DVS::VertexBuffer get_wrapper_buffer() const;

private:
    static std::string generate_uid(const std::string&);

    void construct_buffer(VkBufferUsageFlags, VkMemoryPropertyFlags, VkBuffer&, VkDeviceMemory&);

    uint32_t find_mem_type(uint32_t typeFilter, VkMemoryPropertyFlags properties);

protected:
    std::string m_IDTag;
    uint32_t stride;
private:
    VkContext& m_context; 

    DVS::VertexBuffer m_buffer;

    VkBuffer m_vkBuffer;
    VkDeviceMemory m_vkMem;

    VkBuffer m_stagBuffer;
    VkDeviceMemory m_stagingMem;

};