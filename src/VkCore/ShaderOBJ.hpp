#include "vulkan/vulkan.h"

#include <vector>
#include <fstream>
#include <array>

#include "ShaderTypes.hpp"

#include "VulkanDevice.hpp"


/// TO DO
/// add unbinding for shader - !!!!! -- might be better to not be isndie the classes

namespace ShaderOBJ
{
    template<ShaderType>
    struct ShaderStageTraits;

    template<>
    struct ShaderStageTraits<ShaderType::VERTEX>
    {
        static constexpr VkShaderStageFlagBits flag = VK_SHADER_STAGE_VERTEX_BIT;
    };

    template<>
    struct ShaderStageTraits<ShaderType::TCS>
    {
        static constexpr VkShaderStageFlagBits flag = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    };

    template<>
    struct ShaderStageTraits<ShaderType::TES>
    {
        static constexpr VkShaderStageFlagBits flag = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    };

    template<>
    struct ShaderStageTraits<ShaderType::GEOMETRY>
    {
        static constexpr VkShaderStageFlagBits flag = VK_SHADER_STAGE_GEOMETRY_BIT;
    };

    template<>
    struct ShaderStageTraits<ShaderType::FRAGMENT>
    {
        static constexpr VkShaderStageFlagBits flag = VK_SHADER_STAGE_FRAGMENT_BIT;
    };

    template<>
    struct ShaderStageTraits<ShaderType::COMPUTE>
    {
        static constexpr VkShaderStageFlagBits flag = VK_SHADER_STAGE_COMPUTE_BIT;
    };


    template<ShaderType...>
    struct LinkedShaders : std::false_type{};

    template<>
    struct LinkedShaders<
        ShaderType::VERTEX, 
        ShaderType::FRAGMENT
    > : std::true_type {};

    template<>
    struct LinkedShaders<
        ShaderType::VERTEX,
        ShaderType::GEOMETRY,
        ShaderType::FRAGMENT
    > : std::true_type {};

    template<>
    struct LinkedShaders<
        ShaderType::VERTEX,
        ShaderType::TCS,
        ShaderType::TES,
        ShaderType::GEOMETRY,
        ShaderType::FRAGMENT
    > : std::true_type {};

    template<>
    struct LinkedShaders<
        ShaderType::VERTEX,
        ShaderType::TCS,
        ShaderType::TES,
        ShaderType::FRAGMENT
    > : std::true_type {};

    template<>
    struct LinkedShaders<
        ShaderType::COMPUTE
    > : std::true_type {};

    ///=====================================[SHADER CLASS]=====================================///
    template<ShaderType Type>
    class Shader
    {
    public:

        Shader() = default;
        Shader( VkDevice device_, 
                VkShaderStageFlags    next_stage_,
                std::string           shader_name_,
                const std::string& path
                // const VkDescriptorSetLayout *pSetLayouts,
                // const VkPushConstantRange *  pPushConstantRange)
        );
               

            
        VkShaderCreateInfoEXT get_create_info() const { return vk_shader_create_info; }
        VkShaderStageFlagBits get_stage()       const { return stage; }
        void                  set_shader(VkShaderEXT p_shader) { shader = p_shader; }
        void                  build_shader();
        void                  bind_shader(VkCommandBuffer cmd_buf);

        static std::vector<uint32_t> readFile(const std::string& filePath)
        {
            std::ifstream file(filePath, std::ios::ate | std::ios::binary);

            if (!file.is_open())
                throw std::runtime_error("failed to open file!");

            size_t fileSize = static_cast<size_t>(file.tellg());

            if (fileSize % sizeof(uint32_t) != 0)
                throw std::runtime_error("Invalid SPIR-V file!");

            std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

            file.seekg(0);

            file.read(
                reinterpret_cast<char*>(buffer.data()),
                fileSize
            );

            return buffer;
        }


    public:
        static constexpr ShaderType type = Type;
    private:
        VkDevice              device;
        VkShaderStageFlagBits stage = ShaderStageTraits<Type>::flag;
        VkShaderStageFlags    next_stage{};
        VkShaderEXT           shader      = VK_NULL_HANDLE;
        std::string           shader_name = "shader";
        VkShaderCreateInfoEXT vk_shader_create_info{};
        std::vector<uint32_t> code;
    };

    ///========================================================================================///


    template<ShaderType... Stages>
    requires LinkedShaders<Stages...> ::value
    class ShaderSuite
    {
    public:
        ShaderSuite( const VulkanDevice& device, Shader<Stages>&... shaders )
        : m_device(&device),
          m_shaders(&shaders...)
    {}


        void link();
        void bind(VkCommandBuffer cmd_buf);

    private:
        const VulkanDevice*                                  m_device = nullptr;
        std::tuple<Shader<Stages>*...>                       m_shaders;
        std::array<VkShaderEXT, sizeof...(Stages)>           m_shaderEXTs{};
        std::array<VkShaderStageFlagBits, sizeof...(Stages)> m_stages { ShaderStageTraits<Stages>::flag...};
    };


    // Glorious CTAD
    template<ShaderType... Stages>
    ShaderSuite(const VulkanDevice&, Shader<Stages>&...)
        -> ShaderSuite<Stages...>;


    template<ShaderType... Stages>
    requires LinkedShaders<Stages...> ::value
    void ShaderSuite<Stages...>::link()
    {
        std::array<VkShaderCreateInfoEXT, sizeof...(Stages)> shader_create_infos{};

        std::apply
        (
            [&shader_create_infos](auto*... shaders)
            {
                // I am not sure how to do this :DD

                // if((shaders == nullptr || ...))
                // {
                //     LLOGE("Missing shader/s :((\n");
                // }

                std::size_t i = 0;
 
                ((shader_create_infos[i++] = shaders->get_create_info()), ...);
            },
            m_shaders
        );

        for(auto& shader_create : shader_create_infos)
            shader_create.flags |= VK_SHADER_CREATE_LINK_STAGE_BIT_EXT;

        m_device->get_cmd_create_shaders_ext()
        (
            m_device->get(),
            static_cast<uint32_t>(shader_create_infos.size()), 
            shader_create_infos.data(),
            nullptr,
            m_shaderEXTs.data()
        );
        // VK_ASSERT_MSG(vkCreateShadersEXT(m_device->get(), 
        //                                 static_cast<uint32_t>(shader_create_infos.size()), 
        //                                 shader_create_infos.data(),
        //                                 nullptr,
        //                                 m_shaderEXTs.data()), "failed to create shader objects");

        
        std::apply
        (
            [&](auto*... shaders)
            {
                std::size_t i = 0;
                ((shaders->set_shader(m_shaderEXTs[i++])), ...);
            },
            m_shaders
        );
    }

    template<ShaderType... Stages>
    requires LinkedShaders<Stages...> ::value
    void ShaderSuite<Stages...>::bind(VkCommandBuffer cmd_buf)
    {
        m_device->get_cmd_bind_shaders_ext()(
            cmd_buf,
            static_cast<uint32_t>(m_stages.size()),
            m_stages.data(),
            m_shaderEXTs.data()
        );
    }


    template<ShaderType Type>
    Shader<Type>::Shader(   VkDevice device_,
                            VkShaderStageFlags    next_stage_,
                            std::string           shader_name_,
                            const std::string&    path
                            // const VkDescriptorSetLayout *pSetLayouts,
                            // const VkPushConstantRange *  pPushConstantRange)
                )
        {
                    device      = device_;
                    shader_name = shader_name_;
                    next_stage  = next_stage_;

                    code = readFile(path);


                    vk_shader_create_info.sType                  = VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT;
                    vk_shader_create_info.pNext                  = nullptr;
                    vk_shader_create_info.flags                  = 0;
                    vk_shader_create_info.stage                  = stage;
                    vk_shader_create_info.nextStage              = next_stage;
                    vk_shader_create_info.codeType               = VK_SHADER_CODE_TYPE_SPIRV_EXT;
                    vk_shader_create_info.codeSize               = code.size() * sizeof(uint32_t);
                    vk_shader_create_info.pCode                  = code.data();
                    vk_shader_create_info.pName                  = "main";
                    vk_shader_create_info.setLayoutCount         = 0;
                    vk_shader_create_info.pSetLayouts            = nullptr;
                    vk_shader_create_info.pushConstantRangeCount = 0;
                    vk_shader_create_info.pPushConstantRanges    = nullptr;
                    vk_shader_create_info.pSpecializationInfo    = nullptr;

        }

    template<ShaderType Type>
    void Shader<Type>::build_shader()
    {
        VK_ASSERT_MSG(vkCreateShadersEXT(device, 
                                        1, 
                                        &vk_shader_create_info,
                                        nullptr,
                                        &shader), "failed to create shader object");  
    }

    template<ShaderType Type>
    void Shader<Type>::bind_shader(VkCommandBuffer cmd_buf)
    {
        
        // vkCmdBindShadersEXT(cmd_buf, 1, &stage, &shader);
    }
}

