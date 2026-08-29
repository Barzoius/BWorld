#include "vulkan/vulkan.h"

#include <vector>
#include <fstream>

#include "ShaderTypes.hpp"


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

    template<ShaderType Type>
    class Shader
    {
    public:

        Shader() = default;
        Shader( VkShaderStageFlagBits stage_,
                VkShaderStageFlags    next_stage_,
                std::string           shader_name_,
                const std::string& path
                // const VkDescriptorSetLayout *pSetLayouts,
                // const VkPushConstantRange *  pPushConstantRange)
        );
               

            
        VkShaderCreateInfoEXT get_create_info() const
        {
            return vk_shader_create_info;
        }

        void set_shader(VkShaderEXT shader)
        {

        }
       


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
        
        VkShaderStageFlagBits stage{};
        VkShaderStageFlags    next_stage{};
        VkShaderEXT           shader      = VK_NULL_HANDLE;
        std::string           shader_name = "shader";
        VkShaderCreateInfoEXT vk_shader_create_info{};
        std::vector<uint32_t> code;
    };


    // template<class... Shaders>
    // requires LinkedShaders<std::remove_cvref_t<Shaders>::type...> ::value
    // class ShaderSuite
    // {
    // public:
    //     ShaderSuite(Shaders&&... shaders) : m_shaders(std::forward<Shaders>(shaders)...){}

    //     void link_shaders();

    // private:
    //     std::tuple<Shaders...> m_shaders


    //     // if suite owns copies/moved insatnces
    //     //std::tuple<std::remove_cvref_t<Shaders>...> m_shaders
    // };


    template<ShaderType... Stages>
    requires LinkedShaders<Stages...> ::value
    class ShaderSuite
    {
    public:
        ShaderSuite(Shader<Stages>&... shaders) : m_shaders(&shaders...) {}


        void link_shaders();

    private:
        std::tuple<Shader<Stages>*...> m_shaders;


        // if suite owns copies/moved insatnces
        //std::tuple<std::remove_cvref_t<Shaders>...> m_shaders
    };


    // Glorious CTAD
    template<ShaderType... Stages>
    ShaderSuite(Shader<Stages>&...) -> ShaderSuite<Stages...>;











    template<ShaderType Type>
    Shader<Type>::Shader(   VkShaderStageFlagBits stage_,
                            VkShaderStageFlags    next_stage_,
                            std::string           shader_name_,
                            const std::string& path
                            // const VkDescriptorSetLayout *pSetLayouts,
                            // const VkPushConstantRange *  pPushConstantRange)
                )
        {
                    stage       = stage_;
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

}

