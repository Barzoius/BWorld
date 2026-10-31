#pragma once

#include "vulkan/vulkan.h"

#include <vector>

namespace vk_sync
{
    class SemaphorePool
    {
    public:
        SemaphorePool(VkDevice &);

        SemaphorePool(const SemaphorePool &) = delete;
        SemaphorePool(SemaphorePool &&)      = delete;

        SemaphorePool & operator = (const SemaphorePool &) = delete;
        SemaphorePool & operator = (SemaphorePool &&)      = delete;
        
        ~SemaphorePool();
        
        VkSemaphore request_semaphore();
        void reset();
        uint32_t get_active_semaphore_count() const;

    private:
        VkDevice & m_device;
        std::vector<VkSemaphore> m_semaphores;
        std::vector<VkSemaphore> m_free_semaphores;

        uint32_t m_active_count{0};

    };
}