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

    void bindPipeline(VkCommandBuffer&);

    

    void createPiplineLayout();

	VkPipeline get_handle() const;

    void create_pipeline(const GraphicsPipelineDesc&);

    void Destroy();

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


    // VkViewport viewport{};
    // VkRect2D scissor{};
    
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

};