#pragma once

#include <vector>
#include <type_traits>
#include <assert.h>
#include <string>
#include <unordered_map>

#include "vulkan/vulkan.h"

#include "VkLog.hpp"

#include <format>


namespace DDS
{

    enum ElementType
    {
        Uniform,
        Sampler,
        ReadImage,
        WriteImage,
        Storage,
        Array,
        Empty,
    };

    template<ElementType> struct Map;
        
    template<> struct Map<Uniform>
    {
        static constexpr VkDescriptorType vktype = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        static constexpr const char* code        = "UBO";
    };

    template<> struct Map<Sampler>
    {
        static constexpr VkDescriptorType vktype = VK_DESCRIPTOR_TYPE_SAMPLER;
        static constexpr const char* code        = "SMR";
    };

    template<> struct Map<ReadImage>
    {
        static constexpr VkDescriptorType vktype = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        static constexpr const char* code        = "RIMG";

    };

    template<> struct Map<WriteImage>
    {
        static constexpr VkDescriptorType vktype = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        static constexpr const char* code        = "WIMG";

    };

    template<> struct Map<Storage>
    {
        static constexpr VkDescriptorType vktype = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        static constexpr const char* code        = "SSBO";

    };


    //this class will be needed for a possible array option 
    class LayoutElement
    {
    public:
        LayoutElement() noexcept = default;
        LayoutElement(ElementType type) : m_type(type) {}

        ElementType        get_type()  const noexcept { return m_type; }
        VkShaderStageFlags get_flag()  const noexcept { return m_flag; }
        uint32_t           get_count() const noexcept { return m_count; }
        std::string        get_sig()   const noexcept 
        {
            switch (m_type)
            {
                case Uniform:
                    return Map<Uniform>::code;

                case Sampler:
                    return Map<Sampler>::code;

                case ReadImage:
                    return Map<ReadImage>::code;

                case WriteImage:
                    return Map<WriteImage>::code;

                case Storage:
                    return Map<Storage>::code;

                default:
                    assert(false);
                    return "-----";
            }
        }

        VkDescriptorType get_vk_descriptor_type()
        {
            switch (m_type)
            {
                case Uniform:
                    return Map<Uniform>::vktype;

                case Sampler:
                    return Map<Sampler>::vktype;

                case ReadImage:
                    return Map<ReadImage>::vktype;

                case WriteImage:
                    return Map<WriteImage>::vktype;

                case Storage:
                    return Map<Storage>::vktype;

                default:
                    assert(false);
                    return VK_DESCRIPTOR_TYPE_MAX_ENUM;
            }
        }

        void update_shader_flags(VkShaderStageFlags flag) { m_flag = m_flag | flag; }
        void change_shader_flags(VkShaderStageFlags flag) { m_flag = flag; }
    private:
        ElementType        m_type  = Empty;
        VkShaderStageFlags m_flag  = VK_SHADER_STAGE_ALL; // all classic graphic shaders + compute
        uint32_t           m_count = 0;
    };

    class DescriptorLayout
    {
        friend class DescriptorCodex;
    public:
        DescriptorLayout() noexcept = default;
        DescriptorLayout& append(LayoutElement elem) noexcept
        {
            m_layout.emplace_back(elem); return *this;
        }

        VkDescriptorSetLayout resolve(VkDevice dev)
        {
            std::vector<VkDescriptorSetLayoutBinding> bindings;
            bindings.reserve(m_layout.size());

            uint32_t slot = 0;
            for(auto& elem : m_layout)
            {
                if(elem.get_type() == Array)   assert(false); // not implemented yet
                if(elem.get_type() != Uniform) assert(false); // not implemented yet

                VkDescriptorSetLayoutBinding binding =
                {
                    .binding            = slot,
                    .descriptorType     = elem.get_vk_descriptor_type(),
                    .descriptorCount    = 1,
                    .stageFlags         = elem.get_flag(),
                    .pImmutableSamplers = nullptr // relevant for images
                };
                bindings.emplace_back(binding);
                binding = {};
            }

            VkDescriptorSetLayoutCreateInfo layout_info = 
            {
                .sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                .bindingCount = static_cast<uint32_t>(m_layout.size()),
                .pBindings    = bindings.data()
            };

            VK_ASSERT_MSG(vkCreateDescriptorSetLayout(dev, &layout_info, nullptr, &m_descriptorSetLayout), "failed to create descriptor set layout!");

            return m_descriptorSetLayout;
        }

        VkDescriptorSetLayout get_vk_desc_set_layout() const noexcept { return m_descriptorSetLayout; }
        size_t                get_count()              const noexcept { return m_layout.size(); }
        std::string           get_sig()                const noexcept { return m_sig; }

        void gen_sig()
        {
            using namespace std::string_literals;
            auto sig = "St{"s;
            for(auto& elem : m_layout)
                sig += std::format(
                    "{}:{:#x}:{};",
                    elem.get_sig(),
                    elem.get_flag(),
                    elem.get_count()
                );
            sig += "}"s;

            m_sig = std::move(sig);
        }


    private:
        std::vector<LayoutElement> m_layout;
        VkDescriptorSetLayout      m_descriptorSetLayout;
        std::string                m_sig;
    };



    class DescriptorSet
    {
    public:
        void create()
        {
            std::cout<<"WOW\n";
        }
    private:
        DescriptorLayout m_layout;
    };




    class DescriptorAllocator
    {
    public:
        DescriptorAllocator(VkDevice& dev)
        {
            VkDescriptorPoolSize sizes[] = 
            {
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,          50 },
                { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  30 },
                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,          20 },
            };

            VkDescriptorPoolCreateInfo poolInfo{};
            poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolInfo.poolSizeCount = 3;
            poolInfo.pPoolSizes    = sizes;
            poolInfo.maxSets       = 100;

            VkDescriptorPool descriptorPool;

            VK_ASSERT_MSG(vkCreateDescriptorPool(dev,&poolInfo,nullptr,&descriptorPool),
            "Failed to create base descriptor pool!");

            m_pools.emplace_back(descriptorPool);
        }

        VkDescriptorSet allocate(VkDevice& dev, VkDescriptorSetLayout layout, size_t count)
        {
            VkDescriptorSet desc_set;

            std::vector<VkDescriptorSetLayout> layouts(count, layout);

            VkDescriptorSetAllocateInfo allocInfo{};
            allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool     = m_pools[0];
            allocInfo.descriptorSetCount = static_cast<uint32_t>(count);
            allocInfo.pSetLayouts        = layouts.data();

            std::vector<VkDescriptorSet> descriptorSets;
            descriptorSets.resize(count);

            VK_ASSERT_MSG(vkAllocateDescriptorSets(dev, &allocInfo, descriptorSets.data()),
            "failed to allocate descriptor sets!");
        }


    private:
        std::vector<VkDescriptorPool> m_pools;
    };




    class DescriptorCodex
    {
    public:
        static DescriptorLayout resolve(DescriptorLayout) noexcept
        {

        }
    private:
        static DescriptorCodex& get() noexcept 
        {        
            static DescriptorCodex codex;
            return codex;
        };
    public:
        std::unordered_map<std::string, bool> m_map;
    };
}