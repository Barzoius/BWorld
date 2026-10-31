#include "VkCmdBuffer.hpp"

#include "VkCmdPool.hpp"
#include "VulkanDevice.hpp"

VkCmdBuffer::VkCmdBuffer(VkCmdPool& pool) : m_pool{pool}
{
    VkCommandBufferAllocateInfo cmdAllocInfo
    {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    .commandPool = m_pool.get_handle(),

    // these have to be selectable
    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    .commandBufferCount = 1
    };

    VK_ASSERT_MSG(vkAllocateCommandBuffers(m_pool.get_device().get(), &cmdAllocInfo, &m_handle), "failed to create command buffers!");
}



VkCmdPool & VkCmdBuffer::get_pool()   const
{
    return m_pool;
}

VkCommandBuffer VkCmdBuffer::get_handle() const
{
    return m_handle;
}

VkCmdBuffer::~VkCmdBuffer()
{

}

void VkCmdBuffer::begin()
{
    VkCommandBufferBeginInfo info
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };
    
    vkBeginCommandBuffer(m_handle, &info);
}

void VkCmdBuffer::end()
{
    vkEndCommandBuffer(m_handle);
}

void VkCmdBuffer::begin_render(VkRenderingInfo info)
{
    vkCmdBeginRendering(m_handle, &info);
}

void VkCmdBuffer::end_render()
{
    vkCmdEndRendering(m_handle);
}

// SETS

void VkCmdBuffer::set_vertex_input(DVS::VertexLayout layout)
{
    //m_pool.get_device().vkCmdSetVertexInputEXT();

     VkVertexInputBindingDescription2EXT binding{
        .sType     = VK_STRUCTURE_TYPE_VERTEX_INPUT_BINDING_DESCRIPTION_2_EXT,
        .binding   = 0,
        .stride    = (uint32_t)layout.get_size(),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        .divisor   = 1
    };


    std::vector<VkVertexInputAttributeDescription2EXT> descs;
    descs.resize(layout.get_count());
    for(int i = 0; i < descs.size(); i++)
    {
        descs[i].sType    = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT,
        descs[i].binding  = 0; // modulate this later
        descs[i].location = i;
        descs[i].format   = get_vertex_buffer_format_2(layout.resolve_by_index((size_t)i).get_type());
        descs[i].offset   = layout.resolve_by_index((size_t)i).get_offset();
        
    }

    m_pool.get_device().vkCmdSetVertexInputEXT(
        m_handle,
        1,
        &binding,
        static_cast<uint32_t>(descs.size()),
        descs.data()
    );
}

void VkCmdBuffer::set_viewport(int width, int height)
{
    VkViewport viewport{
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(width),
        .height = static_cast<float>(height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    m_pool.get_device().vkCmdSetViewportWithCountEXT(m_handle, 1, &viewport);
}

void VkCmdBuffer::set_scissor(int width, int height)
{
    VkRect2D scissor{
        .offset = {0, 0},
        .extent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        }
    };

    m_pool.get_device().vkCmdSetScissorWithCountEXT(m_handle, 1, &scissor);
    
}

void VkCmdBuffer::set_cull_mode()
{
    m_pool.get_device().vkCmdSetCullModeEXT(m_handle, VK_CULL_MODE_BACK_BIT);
}
void VkCmdBuffer::set_front_face()
{
    m_pool.get_device().vkCmdSetFrontFaceEXT(m_handle,  VK_FRONT_FACE_CLOCKWISE);
}

void VkCmdBuffer::set_polygon_mode()
{
    m_pool.get_device().vkCmdSetPolygonModeEXT(m_handle, VK_POLYGON_MODE_FILL);
}
void VkCmdBuffer::set_topology()
{
    m_pool.get_device().vkCmdSetPrimitiveTopologyEXT(m_handle, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
}

void VkCmdBuffer::set_primitive_restart()
{
    m_pool.get_device().vkCmdSetPrimitiveRestartEnableEXT(m_handle, VK_FALSE);
}
void VkCmdBuffer::set_alpha_to_coverage()
{
    m_pool.get_device().vkCmdSetAlphaToCoverageEnableEXT(m_handle, VK_FALSE);
}
void VkCmdBuffer::set_stencil_test()
{
    m_pool.get_device().vkCmdSetStencilTestEnableEXT(m_handle, VK_FALSE);
}

void VkCmdBuffer::set_sample_mask()
{
    const uint32_t sampleMask = 0xFF;
    m_pool.get_device().vkCmdSetSampleMaskEXT(m_handle, VK_SAMPLE_COUNT_1_BIT, &sampleMask);
}


void VkCmdBuffer::set_rasterizer_discard()
{
    m_pool.get_device().vkCmdSetRasterizerDiscardEnableEXT(m_handle, VK_FALSE);
}
void VkCmdBuffer::set_rasterization_samples()
{
    m_pool.get_device().vkCmdSetRasterizationSamplesEXT(m_handle, VK_SAMPLE_COUNT_1_BIT);
}



void VkCmdBuffer::set_color_blend()
{
    const VkBool32 colorBlendEnables = false;

    const VkColorBlendEquationEXT colorBlendEquation{};

    m_pool.get_device().vkCmdSetColorBlendEnableEXT(m_handle, 0, 1, &colorBlendEnables);
}
void VkCmdBuffer::set_color_write()
{
    const VkColorComponentFlags colorBlendComponentFlags = 0xf;
    m_pool.get_device().vkCmdSetColorWriteMaskEXT(m_handle, 0, 1, &colorBlendComponentFlags);
}



void VkCmdBuffer::set_depth_bias()
{
    m_pool.get_device().vkCmdSetDepthBiasEnableEXT(m_handle, VK_FALSE);
}
void VkCmdBuffer::set_depth_test()
{
    m_pool.get_device().vkCmdSetDepthTestEnableEXT(m_handle, VK_FALSE);
}
void VkCmdBuffer::set_depth_write()
{
    m_pool.get_device().vkCmdSetDepthWriteEnableEXT(m_handle, VK_FALSE);
}
void VkCmdBuffer::set_depth_compare_op()
{
    m_pool.get_device().vkCmdSetDepthCompareOpEXT(m_handle, VK_COMPARE_OP_LESS);
}


// BINDS


void VkCmdBuffer::bind_vertex_buffers(const Buffer & buff, VkDeviceSize offset)
{
    vkCmdBindVertexBuffers(
        m_handle,
        0,
        1,
        &buff.get_handle(),
        &offset
    );
}

void VkCmdBuffer::bind_vertex_buffers(const BufferPtr & buff, VkDeviceSize offset)
{
    vkCmdBindVertexBuffers(
        m_handle,
        0,
        1,
        &buff.get()->get_handle(),
        &offset
    );
}

void VkCmdBuffer::bind_index_buffers(const Buffer & buff)
{
    vkCmdBindIndexBuffer(
        m_handle,
        buff.get_handle(),
        0,
        VK_INDEX_TYPE_UINT16
    );
}


void VkCmdBuffer::bind_index_buffers(const BufferPtr & buff)
{
    vkCmdBindIndexBuffer(
        m_handle,
        buff.get()->get_handle(),
        0,
        VK_INDEX_TYPE_UINT16
    );
}
  
void bind_vertex_buffers(uint32_t, std::span<const Buffer>)
{

}

void bind_index_buffers(uint32_t, std::span<const Buffer>)
{

}