#include "Allocated.hpp"

namespace alloc
{

VmaAllocator &get_memory_allocator()
{
	static VmaAllocator memory_allocator = VK_NULL_HANDLE;
	return memory_allocator;
}

void initialize_vma_allocator(VkContext& ctx)
{
    //set more flags
    
    VmaAllocatorCreateInfo info{};
    info.instance       = ctx.get_instance().get_handle();
    info.physicalDevice = ctx.get_device().getPhyD();
    info.device         = ctx.get_device().get();
    info.flags          = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

    auto &allocator = get_memory_allocator();

    if(allocator == VK_NULL_HANDLE)
        VK_ASSERT_MSG(vmaCreateAllocator(&info, &get_memory_allocator()), 
        "Failed to create the VMA allocator!");
}


void destroy_vma_allocator()
{
    auto &allocator = get_memory_allocator();
    if(allocator != VK_NULL_HANDLE)
    {
        VmaTotalStatistics stats;
        vmaCalculateStatistics(allocator, &stats);
        LLOGI("Total device memory leaked: {} bytes.", stats.total.statistics.allocationBytes);
        vmaDestroyAllocator(allocator);
        allocator = VK_NULL_HANDLE;
    }
}

}