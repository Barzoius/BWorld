add_library(VkCore STATIC
            VkRenderer.cpp
            VkRenderer.hpp
            
            VulkanQueue.hpp
            VulkanQueue.cpp

            VulkanDevice.hpp
            VulkanDevice.cpp
            

            Instance.hpp
            Instance.cpp

            VulkanSwapchain.hpp
            VulkanSwapchain.cpp

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
    PRIVATE glfw
)


target_include_directories(VkCore PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(VkCore PUBLIC ${Vulkan_LIBRARIES})

target_include_directories(VkCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

