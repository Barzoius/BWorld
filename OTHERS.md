## Basic Vulkan Renderer Dependency Graph 

*  VkInstance
* │
* ├── VkSurfaceKHR (optional, for presentation)
* │
* └── VkPhysicalDevice
    * │
    * └── VkDevice (Logical Device)
        * │
        * ├── VkQueue(s)
        * │
        * ├── VkSwapchainKHR (depends on Surface)
        * │&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;       ├── VkImage (swapchain images)
        * │&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;       └── VkImageView
        * │
        * ├── VkCommandPool
        * │&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;       └── VkCommandBuffer
        * │
        * ├── VkBuffer / VkImage
        * │&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;       └── VkDeviceMemory
        * │
        * ├── VkImageView (for textures, attachments)
        * │
        * ├── VkRenderPass
        * │&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;       └── VkFramebuffer
        * │&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;              └── uses ImageViews
        * │
        * ├── VkDescriptorSetLayout
        * │&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;       └── VkDescriptorPool
        * │&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;              └── VkDescriptorSet
        * │
        * ├── VkPipelineLayout
        * │&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;       └── VkPipeline
        * │
        * ├── VkShaderModule
        * │
        * └── Sync Objects
            * ├── VkFence
            * └── VkSemaphore


## Basic Vulkan Renderer Dependency Table 

| Component                 | Depends On                           | Purpose                              |
| ------------------------- | ------------------------------------ | ------------------------------------ |
| **VkInstance**            | —                                    | Entry point to Vulkan                |
| **VkSurfaceKHR**          | VkInstance                           | Window/system integration            |
| **VkPhysicalDevice**      | VkInstance                           | Represents GPU                       |
| **VkDevice (Logical)**    | VkPhysicalDevice                     | Interface to GPU                     |
| **VkQueue**               | VkDevice                             | Submits work to GPU                  |
| **VkSwapchainKHR**        | VkDevice, VkSurfaceKHR               | Manages presentation images          |
| **VkImage (swapchain)**   | VkSwapchainKHR                       | Framebuffer images                   |
| **VkImageView**           | VkImage                              | View into image data                 |
| **VkBuffer**              | VkDevice                             | Raw memory storage (vertices, etc.)  |
| **VkDeviceMemory**        | VkDevice                             | Allocated GPU memory                 |
| **VkCommandPool**         | VkDevice, QueueFamily                | Allocates command buffers            |
| **VkCommandBuffer**       | VkCommandPool                        | Records GPU commands                 |
| **VkRenderPass**          | VkDevice                             | Defines render stages & attachments  |
| **VkFramebuffer**         | VkRenderPass, VkImageView            | Actual render targets                |
| **VkShaderModule**        | VkDevice                             | Compiled shader                      |
| **VkPipelineLayout**      | VkDevice, DescriptorSetLayouts       | Interface between shader + resources |
| **VkPipeline**            | VkDevice, RenderPass, PipelineLayout | Fully configured GPU pipeline        |
| **VkDescriptorSetLayout** | VkDevice                             | Defines resource bindings            |
| **VkDescriptorPool**      | VkDevice                             | Allocates descriptor sets            |
| **VkDescriptorSet**       | DescriptorPool, Layout               | Binds buffers/images to shaders      |
| **VkSemaphore**           | VkDevice                             | GPU-GPU sync                         |
| **VkFence**               | VkDevice                             | CPU-GPU sync                         |
