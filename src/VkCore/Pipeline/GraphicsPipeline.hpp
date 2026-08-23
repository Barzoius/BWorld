#include "Shader.hpp"
#include "VkContexts/VkSwapChainContext.hpp"


#include <memory>

struct GraphicsPipelineDesc
{
    struct VertexLayout
    {
        VkVertexInputBindingDescription bindDesc;
        std::vector<VkVertexInputAttributeDescription> attrDescs;
    }vertLayout;


    Shader<ShaderType::VERTEX>* vertShader = nullptr;
    Shader<ShaderType::TCS>* tcsShader = nullptr;  
    Shader<ShaderType::TES>* tesShader = nullptr;  
    Shader<ShaderType::GEOMETRY>* geomShader = nullptr;  
    Shader<ShaderType::FRAGMENT>* fragShader = nullptr;  
};

class GraphicsPipeline
{
public:
    GraphicsPipeline(VkContext&, const VkSwapchainContext&);

    ~GraphicsPipeline();

    void bind(VkCommandBuffer&);

    void create_pipline_layout(VkDescriptorSetLayout);

	[[nodiscard]] VkPipeline get_handle() const;

    void create_pipeline(const GraphicsPipelineDesc&, VkDescriptorSetLayout);

    void create_descriptor_pool();
    void create_descriptor_sets();
    void create_descriptor_set_layout();
    void allocate_descriptor_sets();
    void update_descriptor_sets();

    void destroy();

private:
    void initDynamicStates();
    void initVertexInput();
    void initInputAssembly();
    void initViewportState();
    void initRasterizer();
    void initMultisample();
    void initDepthStencil();
    void initColorBlend();

private:
    std::vector<VkDynamicState> dynamicStatesVec;

    VkPipelineDynamicStateCreateInfo dynamicState{};
    VkPipelineVertexInputStateCreateInfo vertexInput{};
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    VkPipelineViewportStateCreateInfo viewportState{};
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    VkPipelineMultisampleStateCreateInfo multisampling{};
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    
private:
    VkContext& context;
    VkSwapchainContext swapchainContext;

    const std::vector<VkDynamicState> dynamicStates = 
    {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };


    VkPipelineLayout pipelineLayout{};
    VkPipeline graphicsPipeline;


    VkDescriptorPool m_descPool;
    VkDescriptorSetLayout m_descSetLayout;
    std::vector<VkDescriptorSet> m_descSets;


};