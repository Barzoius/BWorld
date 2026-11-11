#pragma once

#include "vulkan/vulkan.h"
#include "Renderer.hpp"
#include "VkContext.hpp"

#include <vector>
#include <array>
#include <iostream>

#include "VkContext.hpp"
#include "VkConstants.hpp"


#include "VulkanSwapchain.hpp"

#include "Cmd/VkCmdBuffer.hpp"
#include "Cmd/VkCmdPool.hpp"


#include "Resources/Buffer.hpp"
#include "Resources/Image.hpp"



#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "ShaderOBJ.hpp"

#include "Sync/Barrier.hpp"


#include "VkDescHeapMng.hpp"



struct UBO_TEST{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};


struct CameraUBO
{
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

struct ObjectMODEL
{
    alignas(16) glm::mat4 model;
};


class VkRenderer : public Renderer
{
public:
    VkRenderer(VkContext& ctx) 
    : Renderer(ctx), m_vkContext(ctx), m_heap_manager() {}

    void Initialize(Context&)                override;
    void RenderFrame()                       override;
    void Shutdown()                          override;
    void UpdateResolution(const Resolution&) override;
    void Submit(const RenderObject&)         override;

    void create_swapchain();


    void create_sync_resources();
    void clean_new_sync();
  
    void create_frame_data();
    void render();
    void clean_frame_data();

    void update_global_uniforms();
    void update_entity_unfiorms();

    void clean_swapchain_v2();
    void recreate_swapchain_v2();

    //just for now its sets all states no costomization
    void set_all_states(VkCommandBuffer);

private:
    VkContext&                        m_vkContext;
    std::unique_ptr<VulkanSwapchain>  swapchain;
    
    std::vector<VkFence> m_present_fences;
    
    bool m_framebufferResized  = false;
    bool m_swapchainRecreation = false;

private:
    //------------------------------------[SYNCHRONIZATION DATA]---------------------------------//

    struct frame_data
    {
        VkCmdPool   s_pool;
        VkCmdBuffer s_buffer;
        VkSemaphore s_imgAcquiredSmph = nullptr;

        frame_data(VulkanDevice& device)
        : s_pool(device, device.get_device_indices().s_graphics.value()),
          s_buffer(s_pool) {}
    };
    std::vector<frame_data> m_frameResources2;

    uint32_t currentFrame    = 0;
    uint64_t nextSignalValue = MAX_FRAMES_IN_FLIGHT + 1;


    VkSemaphore timelineSmph = nullptr; // cpu-gpu
    std::vector<VkSemaphore> m_renderCompleteSmphs;//should move to swapchain
    
private:
    struct data{};
    template<std::size_t... I>
    static std::array<data, sizeof...(I)> 
    create_frame_resources(std::index_sequence<I...>)
    {
        return std::array<data, sizeof...(I)>{
            (static_cast<void>(I), data{})...
        };
    }

private: //policies

using present_policy = vk_sync::ImageMem_2
    <vk_sync::Stage::ColorAttachment,vk_sync::Action::ColorWrite,
    vk_sync::Stage::None, vk_sync::Action::Zero>;





public: // for testing

    struct GPU_Mesh
    {
        BufferPtr vertex_buffer;
        BufferPtr index_buffer;
        uint32_t index_count = 0;

        ObjectMODEL model;
        BufferPtr model_buff;
    };
    std::vector<RenderObject> m_render_objects;
    std::vector<GPU_Mesh> m_meshes;

    CameraUBO m_cam_data{};
    BufferPtr m_camera_ubo;

    void create_camera_ubo();
    
    std::unique_ptr<ShaderOBJ::Shader<ShaderType::VERTEX> >   vertex_obj;
        std::unique_ptr<ShaderOBJ::Shader<ShaderType::FRAGMENT> > fragment_obj;
        std::unique_ptr<
            ShaderOBJ::ShaderSuite<
                ShaderType::VERTEX,
                ShaderType::FRAGMENT
            >
        > suite;

    VkDescHeapMng m_heap_manager;



    UBO_TEST ubo{};
    BufferPtr m_uniform_buff_ptr;

    const char* img_data;
    ImagePtr m_test_image;

    VkImage     depthImage     = nullptr;
    VkImageView depthImageView = nullptr;

public:
    static VkFormat get_vertex_buffer_format(DVS::VertexLayout::ElementType type) noexcept
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
