#pragma once

#include <vector>
#include <type_traits>
#include "vulkan/vulkan.h"
#include <assert.h>
#include <string>


namespace DVS
{
    struct RGBACOLOR
    {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
    };

    struct VKFLOAT2
    {
        float x;
        float y;
    };

    struct VKFLOAT3
    {
        float x;
        float y;
        float z;
    };

    struct VKFLOAT4
    {
        float x;
        float y;
        float z;
        float w;
    };

    class VertexLayout
    {
    public:
        enum ElementType
        {
            Position2D,
            Position3D,
            Texture2D,
            Normal,
            Tangent,
            Bitangent,
            Float3Color,
            Float4Color,
            RGBAColor,
        };

        template<ElementType> struct Map;
        template<> struct Map<Position2D>
        {
            using SysType = VKFLOAT2;
            static constexpr VkFormat vkFormat = VK_FORMAT_R32G32_SFLOAT;
            static constexpr const char* semantic = "Position";
            static constexpr const char* code = "P2";
        };

        template<ElementType> struct Map;
        template<> struct Map<Position3D>
        {
            using SysType = VKFLOAT3;
            static constexpr VkFormat vkFormat = VK_FORMAT_R32G32B32_SFLOAT;
            static constexpr const char* semantic = "Position";
            static constexpr const char* code = "P3";
        };
        template<> struct Map<Texture2D>
        {
            using SysType = VKFLOAT2;
            static constexpr VkFormat vkFormat = VK_FORMAT_R32G32_SFLOAT;
            static constexpr const char* semantic = "Texcoord";
            static constexpr const char* code = "TEX";
        };
        template<> struct Map<Normal>
        {
            using SysType = VKFLOAT3;
            static constexpr VkFormat vkFormat = VK_FORMAT_R32G32B32_SFLOAT;
            static constexpr const char* semantic = "Normal";
            static constexpr const char* code = "N";
        };
        template<> struct Map<Tangent>
        {
            using SysType = VKFLOAT3;
            static constexpr VkFormat vkFormat = VK_FORMAT_R32G32B32_SFLOAT;
            static constexpr const char* semantic = "Tangent";
            static constexpr const char* code = "NT";
        };
        template<> struct Map<Bitangent>
        {
            using SysType = VKFLOAT3;
            static constexpr VkFormat vkFormat = VK_FORMAT_R32G32B32_SFLOAT;
            static constexpr const char* semantic = "Bitangent";
            static constexpr const char* code = "NB";
        };
        template<> struct Map<Float3Color>
        {
            using SysType = VKFLOAT3;
            static constexpr VkFormat vkFormat = VK_FORMAT_R32G32B32_SFLOAT;
            static constexpr const char* semantic = "Color";
            static constexpr const char* code = "C3";
        };
        template<> struct Map<Float4Color>
        {
            using SysType = VKFLOAT4;
            static constexpr VkFormat vkFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
            static constexpr const char* semantic = "Color";
            static constexpr const char* code = "C4";
        };
        template<> struct Map<RGBAColor>
        {
            using SysType = DVS::RGBACOLOR;
            static constexpr VkFormat vkFormat = VK_FORMAT_R8G8B8A8_UNORM;
            static constexpr const char* semantic = "Color";
            static constexpr const char* code = "P8";
        };

        class Element
        {
        public:
            Element(ElementType p_type, size_t p_offset) : type(p_type), offset(p_offset){}

            size_t get_offset_after() const noexcept { return offset + get_type_size();}
            size_t get_offset() const noexcept { return offset; }
            size_t get_type_size() const noexcept { return size_of(type); }

            static constexpr size_t size_of(ElementType type) noexcept
            {
                switch(type)
                {
                    case Position2D:
                        return sizeof(VKFLOAT2);
                    case Position3D:
                        return sizeof(VKFLOAT3);
                    case Texture2D:
                        return sizeof(VKFLOAT2);
                    case Normal:
                        return sizeof(VKFLOAT3);
                    case Tangent:
                        return sizeof(VKFLOAT3);
                    case Bitangent:
                        return sizeof(VKFLOAT3);
                    case Float3Color:
                        return sizeof(VKFLOAT3);
                    case Float4Color:
                        return sizeof(VKFLOAT4);
                    case RGBAColor:
                        return sizeof(RGBACOLOR);

                }
            }

            ElementType get_type() const noexcept { return type; }

            VkFormat get_format() const noexcept 
            {
                switch(type)
                    {
                        case Position2D:
                            return Map<Position2D>::vkFormat;
                        case Position3D:
                            return Map<Position3D>::vkFormat;
                        case Texture2D:
                            return Map<Texture2D>::vkFormat;
                        case Normal:
                            return Map<Normal>::vkFormat;
                        case Tangent:
                            return Map<Tangent>::vkFormat;          
                        case Bitangent:
                            return Map<Bitangent>::vkFormat;
                        case Float3Color:
                            return Map<Float3Color>::vkFormat;
                        case Float4Color:
                            return Map<Float4Color>::vkFormat;   
                        case RGBAColor:
                            return Map<RGBAColor>::vkFormat;               
                    }

                    
                assert("Invalid element format" && false);
                return VK_FORMAT_UNDEFINED;
            }

            const char* get_code() const noexcept
            {
                switch (type)
                {
                    case Position2D:
                        return Map<Position2D>::code;
                    case Position3D:
                        return Map<Position3D>::code;
                    case Texture2D:
                        return Map<Texture2D>::code;
                    case Normal:
                        return Map<Normal>::code;
                    case Tangent:
                        return Map<Tangent>::code;
                    case Bitangent:
                        return Map<Bitangent>::code;
                    case Float3Color:
                        return Map<Float3Color>::code;
                    case Float4Color:
                        return Map<Float4Color>::code;
                    case RGBAColor:
                        return Map<RGBAColor>::code;
                }

                assert("Invalid element type" && false);
                return "Invalid";
            }
        private:

        
        private:
            ElementType type;
            size_t offset;
        };

    public:
        template<ElementType Type>
        const Element& resolve() const noexcept
        {
            for(auto& e : m_elements)
            {
                if(e.get_type() == Type)
                    return e;
            }
            assert("Couldn't match element type" && false);
            return m_elements.front();
        }

        const Element& resolve_by_index(size_t p_index) const noexcept
        {
            return m_elements[p_index];
        }

        VertexLayout& append(ElementType type) noexcept
        {
            m_elements.emplace_back(type, get_size());
            return *this;
        }

        size_t get_size() const noexcept 
        { 
            return m_elements.empty() ? 0u : m_elements.back().get_offset_after();
        }

        size_t get_count() const noexcept { return m_elements.size(); }


        std::string get_code() const noexcept
        {
            std::string code;

            for(const auto& e : m_elements)
            {
                code += e.get_code();
            }
            return code;
        }

    private:
        std::vector<Element> m_elements;
    };


    class Vertex
    {
        friend class VertexBuffer;
    public:
        template<VertexLayout::ElementType Type>
        auto& attribute() noexcept
        {
            const auto& element = m_layout.resolve<Type>();
            auto pAttribute = m_pData + element.get_offset();

            return *reinterpret_cast<typename VertexLayout::Map<Type>::SysType*>(pAttribute);
        }

        template<typename T>
        void set_attribute_by_index(size_t p_index, T&& p_value) noexcept
        {
            const auto& element = m_layout.resolve_by_index(p_index);
            auto pAttribute = m_pData + element.get_offset();

            switch(element.get_type())
            {
                case VertexLayout::Position2D:
                    set_attribute<VertexLayout::Position2D>(pAttribute, std::forward<T>(p_value));
                    break;
                case VertexLayout::Position3D:
                    set_attribute<VertexLayout::Position3D>(pAttribute, std::forward<T>(p_value));
                    break;
                case VertexLayout::Texture2D:
                    set_attribute<VertexLayout::Texture2D>(pAttribute, std::forward<T>(p_value));
                    break;       
                case VertexLayout::Normal:
                    set_attribute<VertexLayout::Normal>(pAttribute, std::forward<T>(p_value));
                    break;    
                case VertexLayout::Tangent:
                    set_attribute<VertexLayout::Tangent>(pAttribute, std::forward<T>(p_value));
                    break;  
                case VertexLayout::Bitangent:
                    set_attribute<VertexLayout::Bitangent>(pAttribute, std::forward<T>(p_value));
                    break;  
                case VertexLayout::Float3Color:
                    set_attribute<VertexLayout::Float3Color>(pAttribute, std::forward<T>(p_value));
                    break;
                case VertexLayout::Float4Color:
                    set_attribute<VertexLayout::Float4Color>(pAttribute, std::forward<T>(p_value));
                    break;
                case VertexLayout::RGBAColor:
                    set_attribute<VertexLayout::RGBAColor>(pAttribute, std::forward<T>(p_value));
                    break;
                default:
                    assert("Bad element type" && false);
            }
        }
    
    protected:
        Vertex(char* p_pData, const VertexLayout& p_layout) noexcept
            : m_pData(p_pData), m_layout(p_layout)
        {
            assert(p_pData != nullptr);
        }
    
    private:
        template<typename First, typename...Rest>
        void set_attribute_by_index(size_t p_index, First&& first, Rest&&...rest) noexcept
        {
            set_attribute_by_index(p_index, std::forward<First>(first));
            set_attribute_by_index(p_index + 1, std::forward<Rest>(rest)...);   
        }

        template<VertexLayout::ElementType DestLayoutType, typename SrcLayoutType>
        void set_attribute(char* p_pAttribute, SrcLayoutType&& p_val) noexcept
        {
            using Dest = typename VertexLayout::Map<DestLayoutType>::SysType;
            if constexpr(std::is_assignable<Dest, SrcLayoutType>::value)
            {
                *reinterpret_cast<Dest*>(p_pAttribute) = p_val;
            }
            else
            {
                assert("Parameter attribute type mismatch" && false);
            }
        }
        
    private:
        char* m_pData = nullptr;
        const VertexLayout& m_layout;
    };


    class VertexBuffer
    {
    public:
        VertexBuffer(VertexLayout p_layout, size_t p_size = 0u) noexcept 
            : m_layout(std::move(p_layout))
        {
            resize(p_size);
        }

        void resize(size_t p_size) noexcept
        {
            const auto size = get_size_in_vertices();
            if (size < p_size)
            {
                m_buffer.resize(m_buffer.size() + m_layout.get_size() * (p_size - size));
            }
        }

        const char* get_data() const noexcept { return m_buffer.data(); }
        const VertexLayout& get_layout() const noexcept { return m_layout; }
        size_t get_size_in_vertices() const noexcept { return m_buffer.size() / m_layout.get_size(); }
        size_t get_size_in_bytes() const noexcept { return m_buffer.size(); }

        template<typename ...Params>
        void emplace_back(Params&&... params) noexcept
        {
            assert(sizeof...(params) == m_layout.get_count() && "Param count doesn't match number of vertex elements");
            m_buffer.resize(m_buffer.size() + m_layout.get_size());
            back().set_attribute_by_index(0u, std::forward<Params>(params)...);
        }

        Vertex back() noexcept
        {
            assert(m_buffer.size() != 0u);
            return Vertex{ m_buffer.data() + m_buffer.size() - m_layout.get_size(), m_layout };
        }

        Vertex front() noexcept
        {
            assert(m_buffer.size() != 0u);
            return Vertex{ m_buffer.data(), m_layout };
        }

        Vertex operator[](size_t p_index) noexcept
        {
            assert(p_index < get_size_in_vertices());
            return Vertex{ m_buffer.data() + m_layout.get_size() * p_index, m_layout };
        }


        // i think i can make these 2 static

        VkVertexInputBindingDescription get_bind_desc() noexcept
        {
            VkVertexInputBindingDescription bindDesc{};
            bindDesc.binding = 0; // modulate this later
            bindDesc.stride = (uint32_t)this->get_size_in_bytes();
            bindDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // modulate this later

            return bindDesc;
        }


        std::vector<VkVertexInputAttributeDescription> get_attr_desc()
        {
            std::vector<VkVertexInputAttributeDescription> descs;
            descs.resize(m_layout.get_count());
            for(int i = 0; i < descs.size(); i++)
            {
                descs[i].binding = 0; // modulate this later
                descs[i].location = i;
                descs[i].format = this -> get_layout().resolve_by_index((size_t)i).get_format();
                descs[i].offset = this -> get_layout().resolve_by_index((size_t)i).get_offset();
            }

            return descs;
        }
    private:
        std::vector<char> m_buffer;
        VertexLayout m_layout;
    };
};