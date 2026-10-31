#include "Semaphore.hpp"
#include "VkLog.hpp"

namespace vk_sync
{

    SemaphorePool::SemaphorePool(VkDevice & dev) : m_device{dev} {}

    SemaphorePool::~SemaphorePool()
    {
        reset();

        for (VkSemaphore semaphore : m_semaphores)
            vkDestroySemaphore(m_device, semaphore, nullptr);

        m_semaphores.clear();
    }

    VkSemaphore SemaphorePool::request_semaphore()
    {
        if(m_active_count < m_semaphores.size())
            return m_semaphores[m_active_count++];
        
        VkSemaphore semaphore{VK_NULL_HANDLE};
        VkSemaphoreCreateInfo create_info{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

        VK_ASSERT_MSG(vkCreateSemaphore(m_device, &create_info, nullptr, &semaphore), "Failed to create semaphore");

        m_semaphores.push_back(semaphore);
        m_active_count++;
        
        return semaphore;

    }

    void SemaphorePool::reset()
    {

    }

    uint32_t SemaphorePool::get_active_semaphore_count() const
    {
        return m_active_count;
    }
}