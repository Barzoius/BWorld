cmake_minimum_required(VERSION 3.16)

project(BWorld LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(BWorld)

add_executable(${PROJECT_NAME} src/entry_point.cpp)

#---------------------------DOWNLOAD_ALL_SUBMODULES---------------------------#
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Updating Git submodules...")
        execute_process(
            COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE GIT_SUBMOD_RESULT
        )
        if(NOT GIT_SUBMOD_RESULT EQUAL 0)
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules manually")
        endif()
    endif()
endif()
#----------------------------CHECK_ALL_SUBMODULES-----------------------------#  
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/include/glfw/CMakeLists.txt")
    message(FATAL_ERROR "GLFW submodules were not downloaded. GIT_SUBMODULE was turned off or failed")
endif()         

# Find Vulkan ( windows -- C:\VulkanSDK\1.4.321.1 )
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


add_subdirectory(include/GLFW)
add_subdirectory(src/BEngine)
add_subdirectory(src/VkCore)



target_include_directories(${PROJECT_NAME} PRIVATE ${Vulkan_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    BEngine
    VkCore
    ${Vulkan_LIBRARIES}
)
