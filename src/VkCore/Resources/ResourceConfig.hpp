#pragma once

#include "vulkan/vulkan.h"
#include "VMA/vk_mem_alloc.hpp"

#include <string>
#include <vector>

template<typename RType, typename RInfoType>
class RConfig
{
public:
    VmaAllocationCreateInfo const & get_allocation_create_info() const;
    RInfoType               const & get_create_info()            const;

    std::string             const & get_debug_name()             const;
    
    RType                   & with_debug_name(const std::string &name);
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


template<typename RType, typename RInfoType>
inline RConfig<RType, RInfoType>::RConfig(const RInfoType & info)
: m_create_info{info} { m_alloc_create_info.usage = VMA_MEMORY_USAGE_AUTO; }


template<typename RType, typename RInfoType>
inline VmaAllocationCreateInfo const & RConfig<RType, RInfoType>::get_allocation_create_info() const
{
    return m_alloc_create_info;
}

template<typename RType, typename RInfoType>
inline RInfoType const & RConfig<RType, RInfoType>::get_create_info() const
{
    return m_create_info;
}

template<typename RType, typename RInfoType>
inline RInfoType & RConfig<RType, RInfoType>::get_create_info()
{
    return m_create_info;
}

template<typename RType, typename RInfoType>
inline std::string const & RConfig<RType, RInfoType>::get_debug_name() const
{
    return m_debug_name;
}


template<typename RType, typename RInfoType>
RType & RConfig<RType, RInfoType>::with_debug_name(const std::string &name)
{
    m_debug_name = name;
	return *static_cast<RType*>(this);
}

template<typename RType, typename RInfoType>
RType & RConfig<RType, RInfoType>::with_implicit_sharing_mode()
{

    return *static_cast<RType*>(this);
}


template<typename RType, typename RInfoType>
RType & RConfig<RType, RInfoType>::with_memory_type_bits(uint32_t type_bits)
{
	m_alloc_create_info.memoryTypeBits = type_bits;
	return *static_cast<RType*>(this);
}

template<typename RType, typename RInfoType>
RType & RConfig<RType, RInfoType>::with_queue_families(uint32_t count, const uint32_t *family_indices)
{

    m_create_info.queueFamilyIndexCount = count;
    m_create_info.pQueueFamilyIndices   = family_indices;
    return *static_cast<RType*>(this);
}



template<typename RType, typename RInfoType>
RType & RConfig<RType, RInfoType>::with_queue_families(std::vector<uint32_t> const &queue_families)
{
    return with_queue_families(static_cast<uint32_t>(queue_families.size()), queue_families.data());
}

template<typename RType, typename RInfoType>
RType & RConfig<RType, RInfoType>::with_sharing_mode(VkSharingMode sharing_mode)
{
    m_create_info.sharingMode = sharing_mode;
    return *static_cast<RType*>(this);
}

template<typename RType, typename RInfoType>
RType & RConfig<RType, RInfoType>::with_vma_flags(VmaAllocationCreateFlags flags)
{
    m_alloc_create_info.flags = flags;
    return *static_cast<RType*>(this);
}

template<typename RType, typename RInfoType>
RType & RConfig<RType, RInfoType>::with_vma_pool(VmaPool pool)
{
    m_alloc_create_info.pool = pool;
    return *static_cast<RType*>(this);
}

template<typename RType, typename RInfoType>
RType & RConfig<RType, RInfoType>::with_vma_preferred_flags(VkMemoryPropertyFlags flags)
{
    return *static_cast<RType*>(this);
}

template<typename RType, typename RInfoType>
RType & RConfig<RType, RInfoType>::with_vma_required_flags(VkMemoryPropertyFlags flags)
{
    m_alloc_create_info.preferredFlags = flags;
    return *static_cast<RType*>(this);
}

template<typename RType, typename RInfoType>
RType & RConfig<RType, RInfoType>::with_vma_usage(VmaMemoryUsage usage)
{
    m_alloc_create_info.usage = usage;
    return *static_cast<RType*>(this);
}