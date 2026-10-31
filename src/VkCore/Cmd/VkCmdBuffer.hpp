#pragma once

#include "vulkan/vulkan.h"

#include <span>

#include "Resources/Buffer.hpp"
#include "Resources/Image.hpp"

#include "VertexSystem.hpp"

class VkCmdPool;

class VkCmdBuffer
{
public:
    VkCmdBuffer(VkCmdPool&);
    ~VkCmdBuffer();

    VkCmdBuffer(VkCmdBuffer &&)      = default;
    VkCmdBuffer(VkCmdBuffer const &) = delete;

    VkCmdBuffer & operator = (VkCmdBuffer &&)      = delete;
    VkCmdBuffer & operator = (VkCmdBuffer const &) = delete;

    void begin();
    void begin_render(VkRenderingInfo);
    void end();
    void end_render();

    void set_vertex_input(DVS::VertexLayout);
    void set_viewport(int, int);
    void set_scissor(int, int);

    void set_default_states();
    
    void set_cull_mode();
    void set_front_face();
    void set_polygon_mode();
    void set_topology();
    void set_primitive_restart();
    void set_alpha_to_coverage();
    void set_stencil_test();

    void set_sample_mask();

    void set_color_blend();
    void set_color_write();

    void set_rasterizer_discard();
    void set_rasterization_samples();

    void set_depth_bias();
    void set_depth_test();
    void set_depth_write();
    void set_depth_compare_op();



    void bind_vertex_buffers(const BufferPtr &, VkDeviceSize);
    void bind_vertex_buffers(const Buffer &, VkDeviceSize);
    void bind_vertex_buffers(uint32_t, std::span<const Buffer>);
    void bind_index_buffers(const BufferPtr &);
    void bind_index_buffers(const Buffer &);
    void bind_index_buffers(uint32_t, std::span<const Buffer>);
    void bind_resource_heap();

    void bind_shader();
    void bind_linked_shaders();

    void draw();
    void draw_indexed();

    // void copy_buffer(Buffer const &, Buffer const &, VkDeviceSize);
    // void copy_buffer_to_img(Buffer const &, Image const &);
    // void copy_img(Image const &, Image const &);
    // void copy_img_to_buffer(Image const &, Buffer const &);

    VkCmdPool &     get_pool()   const;
    VkCommandBuffer get_handle() const;
private:
    VkCmdPool &     m_pool;
    VkCommandBuffer m_handle;

private:
 static VkFormat get_vertex_buffer_format_2(DVS::VertexLayout::ElementType type) noexcept
    {

        switch(type)
        {
            case DVS::VertexLayout::ElementType::Position2D:
                return VK_FORMAT_R32G32_SFLOAT;
            case DVS::VertexLayout::ElementType::Position3D:
                return VK_FORMAT_R32G32B32_SFLOAT;
            case DVS::VertexLayout::ElementType::Texture2D:
                return VK_FORMAT_R32G32_SFLOAT;
            case DVS::VertexLayout::ElementType::Normal:
                return VK_FORMAT_R32G32B32_SFLOAT;;
            case DVS::VertexLayout::ElementType::Tangent:
                return VK_FORMAT_R32G32B32_SFLOAT;          
            case DVS::VertexLayout::ElementType::Bitangent:
                return VK_FORMAT_R32G32B32_SFLOAT;
            case DVS::VertexLayout::ElementType::Float3Color:
                return VK_FORMAT_R32G32B32_SFLOAT;
            case DVS::VertexLayout::ElementType::Float4Color:
                return VK_FORMAT_R32G32B32A32_SFLOAT;   
            case DVS::VertexLayout::ElementType::RGBAColor:
                return VK_FORMAT_R8G8B8A8_UNORM;               
        }

                        
        assert("Invalid element format" && false); return VK_FORMAT_UNDEFINED;
        
    }
};