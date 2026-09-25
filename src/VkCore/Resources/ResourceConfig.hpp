#pragma once

#include "vulkan/vulkan.h"
#include "VMA/vk_mem_alloc.hpp"

#include <string>

template<typename RType, typename RInfoType>
class RConfig
{
public:
    VmaAllocationCreateInfo const & get_allocation_create_info() const;
    RInfoType               const & get_create_info()            const;

    std::string             const & get_debug_name()             const;
    
    RType                   & with_implicit_sharing_mode();
    RType                   & with_memory_type_bits(uint32_t type_bits);
    RType                   & with_queue_families(uint32_t count, const uint32_t *family_indices);
    RType                   & with_queue_families(std::vector<uint32_t> const &queue_families);
    RType                   & with_sharing_mode(VkSharingMode sharing_mode);
    RType                   & with_vma_flags(VmaAllocationCreateFlags flags);
    RType                   & with_vma_pool(VmaPool pool);
    RType                   & with_vma_preferred_flags(VkMemoryPropertyFlags flags);
    RType                   & with_vma_required_flags(VkMemoryPropertyFlags flags);
    RType                   & with_vma_usage(VmaMemoryUsage usage);
protected:
    RConfig(const RConfig&) = delete;
    RConfig(const RInfoType &);

    RInfoType & get_create_info();
protected:
    VmaAllocationCreateInfo m_alloc_create_info = {};
    RInfoType               m_create_info       = {};
    std::string             m_debug_name        = {};
};