add_library(VkCore STATIC

            VMA/vk_mem_alloc.h
            VMA/VmaUsage.cpp

            VkContext.hpp
            VkContext.cpp

            VkRenderer.cpp
            VkRenderer.hpp

            VkLog.hpp
            
            VulkanQueue.hpp

            VulkanDevice.hpp
            VulkanDevice.cpp
        
            Instance.hpp
            Instance.cpp

            VulkanSwapchain.hpp
            VulkanSwapchain.cpp

            VkContexts/VkSwapChainContext.hpp

            VkUtils.hpp
            VkUtils.cpp

            Queue.hpp
            Queue.cpp

            Commands.hpp



            Systems/System.hpp
            Systems/System.cpp
            
            Descriptors/model_desc.hpp

            Pipeline/GraphicsPipeline.hpp
            Pipeline/GraphicsPipeline.cpp

            Pipeline/ComputePipeline.hpp

            Pipeline/Shader.hpp
            Pipeline/Shader.cpp

            Pipeline/ShaderType.hpp

            Pipeline/RenderPass.hpp
            Pipeline/RenderPass.cpp


            Resources/VertexSystem.hpp

            Resources/VertexBuffer.hpp
            Resources/VertexBuffer.cpp

            Resources/Buffers.hpp
            Resources/Buffers.cpp


            Drawables/Drawable.hpp


            )

target_link_libraries(VkCore PRIVATE BEngine)

# ------------------- Find Vulkan ------------------- #
if(DEFINED ENV{VULKAN_SDK_PATH})
    set(VULKAN_SDK_PATH $ENV{VULKAN_SDK_PATH})
    set(Vulkan_INCLUDE_DIRS "${VULKAN_SDK_PATH}/Include")
    set(Vulkan_LIBRARIES "${VULKAN_SDK_PATH}/Lib/vulkan-1.lib")  # Windows
    set(Vulkan_FOUND TRUE)
    message(STATUS "Using Vulkan SDK from VULKAN_SDK_PATH: ${VULKAN_SDK_PATH}")
else()
    find_package(Vulkan REQUIRED)
    message(STATUS "Found Vulkan: include=${Vulkan_INCLUDE_DIRS}, lib=${Vulkan_LIBRARY}")
endif()

if(NOT Vulkan_FOUND)
    message(FATAL_ERROR "Could not find Vulkan library!")
endif()

target_link_libraries(VkCore
    PRIVATE glfw glm

)


target_include_directories(VkCore PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(VkCore PUBLIC ${Vulkan_LIBRARIES})

target_include_directories(VkCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

