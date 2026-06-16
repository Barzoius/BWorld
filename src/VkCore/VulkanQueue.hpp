#pragma once

#include <vector>

struct GraphicsQueueTraits
{
    static constexpr bool canPresent = true;
    static constexpr bool canTransfer = true;
    static constexpr bool canCompute = true;
};

struct TransferQueueTraits
{
    static constexpr bool canPresent = false;
    static constexpr bool canTransfer = true;
    static constexpr bool canCompute = false;
};

struct ComputeQueueTraits
{
    static constexpr bool canPresent = false;
    static constexpr bool canTransfer = false;
    static constexpr bool canCompute = true;
};

template <typename Traits>
class VulkanQueue
{
public:
    VulkanQueue(uint32_t);

    void init(VkDevice&);
    void submit();
    void wait();
    void present() requires (Traits::canPresent);
    void copy_buffer() requires (Traits::canTransfer);

    VkQueue get_handle() const;
    uint32_t get_index() const;
    float get_priority() const;

private:
    VkQueue m_handle;

    uint32_t m_index;
    float m_priority = 1.0;
};

template <typename Traits>
VulkanQueue<Traits>::VulkanQueue(uint32_t p_index) : m_index(p_index) {}


template <typename Traits>
VkQueue VulkanQueue<Traits>::get_handle() const
{
    return m_handle;
}

template <typename Traits>
uint32_t VulkanQueue<Traits>::get_index() const
{
    return m_index;
}

template <typename Traits>
float VulkanQueue<Traits>::get_priority() const
{
    return m_priority;
}

template <typename Traits>
void VulkanQueue<Traits>::init(VkDevice& p_device)
{
    vkGetDeviceQueue(p_device, m_index, 0, &m_handle);
}
template <typename Traits>
void VulkanQueue<Traits>::submit()
{

}

template <typename Traits>
void VulkanQueue<Traits>::wait()
{
    vkQueueWaitIdle(m_handle);
}


template <typename Traits>
void VulkanQueue<Traits>::present() requires (Traits::canPresent)
{

}

template <typename Traits>
void VulkanQueue<Traits>::copy_buffer() requires (Traits::canTransfer)
{

}


