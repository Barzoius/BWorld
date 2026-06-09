#include "VertexBuffer.hpp"

VertexBuffer::VertexBuffer(VkContext& c, DVS::VertexBuffer vbuf) : m_context(c) , m_buffer(std::move(vbuf))
{
}

void VertexBuffer::create_buffer()
{


    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = m_buffer.get_size_in_bytes();
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(m_context.get_device().get(), &bufferInfo, nullptr, &m_vkBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create vertex buffer!");
    }

    alloc_and_bind_mem();

    void* data;
    vkMapMemory(m_context.get_device().get(), m_vkMem, 0, m_buffer.get_size_in_bytes(), 0, &data);
    memcpy(data, m_buffer.get_data(), (size_t) m_buffer.get_size_in_bytes());
    vkUnmapMemory(m_context.get_device().get(), m_vkMem);

    // VkBuffer stagingBuffer;
    // VkDeviceMemory stagingBufferMemory;

    // construct_buffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    
    // void* data;
    // vkMapMemory(m_context.get_device().get(), stagingBufferMemory, 0, m_buffer.get_size_in_bytes(), 0, &data);
    // memcpy(data, m_buffer.get_data(), (size_t) m_buffer.get_size_in_bytes());
    // vkUnmapMemory(m_context.get_device().get(), stagingBufferMemory);

    // construct_buffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vkBuffer, m_vkMem);
}

void VertexBuffer::construct_buffer(VkBufferUsageFlags p_usage, VkMemoryPropertyFlags p_properties, VkBuffer& p_buffer, VkDeviceMemory& p_bufferMem)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = m_buffer.get_size_in_bytes();
    bufferInfo.usage = p_usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(m_context.get_device().get(), &bufferInfo, nullptr, &m_vkBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create vertex buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_context.get_device().get(), m_vkBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = find_mem_type(memRequirements.memoryTypeBits, p_properties);

    if (vkAllocateMemory(m_context.get_device().get(), &allocInfo, nullptr, &m_vkMem) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    }

    vkBindBufferMemory(m_context.get_device().get(), m_vkBuffer, m_vkMem, 0);
}






void VertexBuffer::destroy_buffer() noexcept
{
    vkDestroyBuffer(m_context.get_device().get(), m_vkBuffer, nullptr);
    vkFreeMemory(m_context.get_device().get(), m_vkMem, nullptr);
}



uint32_t VertexBuffer::find_mem_type(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(m_context.get_device().getPhyD(), &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

void VertexBuffer::alloc_and_bind_mem()
{
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_context.get_device().get(), m_vkBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = find_mem_type(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if (vkAllocateMemory(m_context.get_device().get(), &allocInfo, nullptr, &m_vkMem) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    }

    vkBindBufferMemory(m_context.get_device().get(), m_vkBuffer, m_vkMem, 0);
}


void VertexBuffer::alloc_and_bind_mem(VkBuffer& p_buffer, VkDeviceMemory& p_bufferMem)
{
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_context.get_device().get(), p_buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = find_mem_type(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if (vkAllocateMemory(m_context.get_device().get(), &allocInfo, nullptr, &p_bufferMem) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    }

    vkBindBufferMemory(m_context.get_device().get(), p_buffer, p_bufferMem, 0);
}



VkBuffer VertexBuffer::get_handle(){ return m_vkBuffer; }
VkBuffer VertexBuffer::get_staging_buffer_handle() { return m_stagBuffer; }
size_t VertexBuffer::get_size() { return m_buffer.get_size_in_bytes(); }

VkVertexInputBindingDescription VertexBuffer::get_bind_desc()
{

    VkVertexInputBindingDescription bindDesc{};
    bindDesc.binding = 0; // modulate this later
    bindDesc.stride = (uint32_t)m_buffer.get_layout().get_size();
    bindDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // modulate this later

    return bindDesc;
}
std::vector<VkVertexInputAttributeDescription> VertexBuffer::get_attr_desc()
{

    std::vector<VkVertexInputAttributeDescription> descs;
    descs.resize(m_buffer.get_layout().get_count());
    for(int i = 0; i < descs.size(); i++)
    {
        descs[i].binding = 0; // modulate this later
        descs[i].location = i;
        descs[i].format = m_buffer.get_layout().resolve_by_index((size_t)i).get_format();
        descs[i].offset = m_buffer.get_layout().resolve_by_index((size_t)i).get_offset();
        
    }

    return descs;
}

DVS::VertexBuffer VertexBuffer::get_wrapper_buffer() const
{
    return m_buffer;
}