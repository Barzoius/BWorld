add_library(VkCore STATIC
            VkRenderer.cpp
            
            )

target_include_directories(VkCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})