#pragma once

#include <vector>
#include <type_traits>
#include <assert.h>
#include <string>
#include <unordered_map>
#include <variant>
#include <span>

#include "vulkan/vulkan.h"

#include "VkLog.hpp"
#include "VkContext.hpp"
#include "DescriptorAllocator.hpp"
#include "Resources/Buffers.hpp"

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

    ///=====================================[ELEMENT]=====================================///
    class LayoutElement
    {
    public:
        LayoutElement() noexcept = default;
        LayoutElement(ElementType type) : m_type(type) {}
        LayoutElement(ElementType type, VkShaderStageFlags flag) : m_type(type), m_flag(flag) {}

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

    ///=====================================[LAYOUT]=====================================///
    class Layout
    {
    public:
        Layout() noexcept = default;
        Layout& append(LayoutElement elem) noexcept
        {
            m_layout.emplace_back(elem); return *this;
        }

        VkDescriptorSetLayout resolve(VkDevice dev)
        {
            gen_sig();
            
            std::vector<VkDescriptorSetLayoutBinding> bindings;
            bindings.reserve(m_layout.size());

            uint32_t slot = 0;
            for(auto& elem : m_layout)
            {
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
                slot++;
                binding = {};
            }

            VkDescriptorSetLayoutCreateInfo layout_info = 
            {
                .sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                .bindingCount = static_cast<uint32_t>(m_layout.size()),
                .pBindings    = bindings.data()
            };

            VkDescriptorSetLayout lay;
            VK_ASSERT_MSG(vkCreateDescriptorSetLayout(dev, &layout_info, nullptr, &lay ), "failed to create descriptor set layout!");

            return lay;
        }


        std::vector<LayoutElement> get_layout()        const noexcept {return m_layout; }
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
        std::string                m_sig;
    };


    ///=====================================[CACHE]=====================================///
    // is not a really good idea to make this global especially 
    // if we have multiple devices but is not the case here
    class DescriptorLayoutCache 
    {
    public:
        static VkDescriptorSetLayout resolve(const VkDevice& dev, Layout lay) noexcept
        {
            auto sig  = lay.get_sig();
            auto& map = get().m_map;

            const auto i = map.find(sig);

            if(i != map.end()) return i->second;
            
            auto result = map.insert({std::move(sig), lay.resolve(dev)});

            return result.first -> second;
        }

        static void destroy(const VkDevice& dev)
        {
            for(auto& i : get().m_map) 
            {
                std::cout<<"Destroying layout: "<<i.first<<"|\n";
                vkDestroyDescriptorSetLayout(dev, i.second, nullptr);
            }
            get().m_map.clear();
        }

    private:
        static DescriptorLayoutCache& get() noexcept 
        {        
            static DescriptorLayoutCache cache;
            return cache;
        };
    public:
        std::unordered_map<std::string, VkDescriptorSetLayout> m_map;
    };


    ///=======================================[SET]=======================================///


    struct DescriptorBuffer
    {
        VkBuffer buffer;
        VkDeviceSize offset;
        VkDeviceSize range;
        VkDescriptorType descriptor_type() const noexcept
        {
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        }
    };

    struct DescriptorImage
    {
        VkSampler sampler;
        VkImageView imageView;
        VkImageLayout layout;
        VkDescriptorType descriptor_type() const noexcept
        {
            return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        }
    };

    using DescriptorResource =
        std::variant<
            DescriptorBuffer,
            DescriptorImage,
            VkBufferView
        >;

    class DescriptorSet
    {
    public:
        DescriptorSet(const VkDevice& dev, Layout& lay) : m_layout(lay)
        {
            m_vkLayout = DescriptorLayoutCache::resolve(dev, lay);
            //m_set = alloc.allocate(dev, m_vkLayout);
        }


        void write(const VkDevice& dev, uint32_t binding, const DescriptorBuffer& buffer)
        {
            auto& element = m_layout.get_layout()[binding];
            assert(element.get_vk_descriptor_type() == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER ||
                   element.get_vk_descriptor_type() == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER );

            VkDescriptorBufferInfo info{};
            info.buffer = buffer.buffer;
            info.offset = buffer.offset; //THE FUCK IS THIS FOR
            info.range  = buffer.range;

            VkWriteDescriptorSet write{};
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet = m_set;
            write.dstBinding = binding;
            write.dstArrayElement = 0;
            write.descriptorType = element.get_vk_descriptor_type();
            write.descriptorCount = 1;
            write.pBufferInfo = &info;

            vkUpdateDescriptorSets(
                dev,
                1,
                &write,
                0,
                nullptr
            );
        }

        void write(uint32_t binding, const DescriptorImage& image)
        {

        }

    private:
        Layout                m_layout;
        VkDescriptorSetLayout m_vkLayout;
        VkDescriptorSet       m_set;
    };

}