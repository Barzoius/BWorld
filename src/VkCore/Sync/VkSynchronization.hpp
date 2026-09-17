#include "vulkan/vulkan.h"

#define STAGES           \
    X( Compute )         \
    X( Vertex )          \
    X( Fragment )        \
    X( ColorAttachment ) \
    X( Transfer )        \


#define ACTIONS        \
    X( StorageRead )   \
    X( StorageWrite )  \
    X( SampledRead )   \
    X( VertexRead )    \
    X( IndexRead )     \
    X( UniformRead )   \
    X( ColorWrite )    \
    X( TransferRead )  \
    X( TransferWrite ) \



namespace vk_sync
{
    enum Action
    {
        #define X(action) action,
        ACTIONS
        #undef X
        NO_ACTION,
    };

    enum Stage
    {
        #define X(stage) stage,
        STAGES
        #undef X
        NO_STAGE,
    };


    //====================================[STAGE MAP]====================================//
    template<Stage>  struct SMap { static constexpr bool valid = false; };


    template<> struct SMap<Stage::Compute>
    {
        static constexpr VkPipelineStageFlags2 stage = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
        static constexpr bool valid                  = true;
    };

    template<> struct SMap<Stage::Fragment>
    {
        static constexpr VkPipelineStageFlags2 stage = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
        static constexpr bool valid                  = true;
    };

    template<> struct SMap<Stage::Vertex>
    {
        static constexpr VkPipelineStageFlags2 stage = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
        static constexpr bool valid                         = true;
    };

    template<> struct SMap<Stage::ColorAttachment>
    {
        static constexpr VkPipelineStageFlags2 stage = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        static constexpr bool valid                  = true;
    };

    template<> struct SMap<Stage::Transfer>
    {
        static constexpr VkPipelineStageFlags2 stage = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
        static constexpr bool valid                  = true;
    };

    #define X(stage) static_assert(SMap<stage>::valid, "Stage("#stage") is invalid!");
    STAGES
    #undef X

    //====================================[ACTION MAP]====================================//
    template<Action> struct AMap { static constexpr bool valid = false; };

    template<> struct AMap<Action::StorageRead>
    {
        static constexpr VkAccessFlags2 value = VK_ACCESS_2_SHADER_STORAGE_READ_BIT;
        static constexpr bool valid           = true;
    };

    template<>
    struct AMap<Action::StorageWrite>
    {
        static constexpr VkAccessFlags2 value = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT;
        static constexpr bool valid           = true;
    };

    template<>
    struct AMap<Action::SampledRead>
    {
        static constexpr VkAccessFlags2 value = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT;
        static constexpr bool valid           = true;
    };

    template<>
    struct AMap<Action::VertexRead>
    {
        static constexpr VkAccessFlags2 value = VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT;
        static constexpr bool valid           = true;
    };

    template<>
    struct AMap<Action::IndexRead>
    {
        static constexpr VkAccessFlags2 value = VK_ACCESS_2_INDEX_READ_BIT;
        static constexpr bool valid           = true;
    };

    template<>
    struct AMap<Action::UniformRead>
    {
        static constexpr VkAccessFlags2 value = VK_ACCESS_2_UNIFORM_READ_BIT;
        static constexpr bool valid           = true;
    };

    template<>
    struct AMap<Action::ColorWrite>
    {
        static constexpr VkAccessFlags2 value = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
        static constexpr bool valid           = true;
    };

    template<>
    struct AMap<Action::TransferRead>
    {
        static constexpr VkAccessFlags2 value = VK_ACCESS_2_TRANSFER_READ_BIT;
        static constexpr bool valid           = true;
    };

    template<>
    struct AMap<Action::TransferWrite>
    {
        static constexpr VkAccessFlags2 value = VK_ACCESS_2_TRANSFER_WRITE_BIT;
        static constexpr bool valid           = true;
    };

    #define X(action) static_assert(AMap<action>::valid, "Action("#action") is invalid!");
    ACTIONS 
    #undef X

    //==================================[BARRIER POLICIES]==================================//

    template<Stage SrcS, Action SrcA, Stage DstS, Action DstA>
    struct Mem_2
    {
        VkDependencyInfo get_dependency()
        {
            VkMemoryBarrier2 barrier
            {
                .sType         = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2,
                .srcStageMask  = SMap<SrcS>::stage,
                .srcAccessMask = AMap<SrcA>::value,

                .dstStageMask  = SMap<DstS>::stage,
                .dstAccessMask = AMap<DstA>::value,
            };

            VkDependencyInfo dependency
            {
                .sType              = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                .memoryBarrierCount = 1,
                .pMemoryBarriers    = &barrier
            };

            return dependency;
        }
    };

    template<Stage SrcS, Action SrcA, Stage DstS, Action DstA>
    struct BufferMem_2
    {
        VkDependencyInfo get_dependency()
        {
            VkMemoryBarrier2 barrier
            {
                .sType         = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
                .srcStageMask  = SMap<SrcS>::stage,
                .srcAccessMask = AMap<SrcA>::value,

                .dstStageMask  = SMap<DstS>::stage,
                .dstAccessMask = AMap<DstA>::value,
            };

            VkDependencyInfo dependency
            {
                .sType              = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                .memoryBarrierCount = 1,
                .pMemoryBarriers    = &barrier
            };

            return dependency;
        }
    };

    template<Stage SrcS, Action SrcA, Stage DstS, Action DstA>
    struct ImageMem_2
    {
        VkDependencyInfo get_dependency()
        {
            VkMemoryBarrier2 barrier
            {
                .sType         = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                .srcStageMask  = SMap<SrcS>::stage,
                .srcAccessMask = AMap<SrcA>::value,

                .dstStageMask  = SMap<DstS>::stage,
                .dstAccessMask = AMap<DstA>::value,
            };

            VkDependencyInfo dependency
            {
                .sType              = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                .memoryBarrierCount = 1,
                .pMemoryBarriers    = &barrier
            };

            return dependency;
        }
    };

    template <typename BarrierPolicy>
    class Barrier : public BarrierPolicy
    {
    public:
        // NOTE: calling get_dependency() works only as a MSVC extension
        // for standard cpp u have to use this->get_dependency()
        Barrier() noexcept : m_dependency(this->get_dependency()) {}

        void bind_barrier(VkCommandBuffer& cmd) const noexcept 
        { 
            vkCmdPipelineBarrier2(cmd, &m_dependency);
        }
    private:
        VkDependencyInfo m_dependency;
    };

}

#ifndef VK_SYNC
#undef STAGES
#undef ACTIONS
#endif