#pragma once

#include "vulkan/vulkan.h"
#include "Renderer.hpp"
#include "VkContext.hpp"

#include <vector>
#include <array>
#include <iostream>

#include "VkContext.hpp"
#include "VkContexts/VkSwapChainContext.hpp"
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


#include <cstdlib>

struct UBO_TEST{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
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

    void create_swapchain();


    void create_sync_resources();
    void clean_new_sync();
  
    void create_frame_data();
    void render();
    void clean_frame_data();


    void clean_swapchain_v2();
    void recreate_swapchain_v2();

    //just for now its sets all states no costomization
    void set_all_states(VkCommandBuffer);
    void record_to_buffer(VkCommandBuffer&);

private:
    VkContext&                        m_vkContext;
    std::unique_ptr<VulkanSwapchain>  swapchain;
    VkSwapchainContext                swapchainContext;
    
    bool m_framebufferResized  = false;
    bool m_swapchainRecreation = false;

private:
    //------------------------------------[SYNCHRONIZATION DATA]---------------------------------//
    struct frameData
    {
        VkCommandPool   s_commandPool     = nullptr;
        VkCommandBuffer s_commandBuffer   = nullptr;
        VkSemaphore     s_imgAcquiredSmph = nullptr;
    };
    std::array<frameData, MAX_FRAMES_IN_FLIGHT> m_frameResources;


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
    std::unique_ptr<ShaderOBJ::Shader<ShaderType::VERTEX> >   vertex_obj;
        std::unique_ptr<ShaderOBJ::Shader<ShaderType::FRAGMENT> > fragment_obj;
        std::unique_ptr<
            ShaderOBJ::ShaderSuite<
                ShaderType::VERTEX,
                ShaderType::FRAGMENT
            >
        > suite;

    VkDescHeapMng m_heap_manager;

    BufferPtr m_vert_buff_ptr;
    BufferPtr m_index_buff_ptr;


    std::vector<DVS::VertexBuffer> input_vertex_buffers;


    const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0
    };
    void construct_vertex_buffer();



    void update_uniform_buffer(uint32_t currentImage);


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
