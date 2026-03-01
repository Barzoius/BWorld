cmake_minimum_required(VERSION 3.16)

project(BWorld LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(BWorld)

add_executable(${PROJECT_NAME} src/entry_point.cpp)

#---------------------------[DOWNLOAD_ALL_SUBMODULES]---------------------------#
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
#----------------------------[CHECK_ALL_SUBMODULES]-----------------------------#  
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/include/glfw/CMakeLists.txt")
    message(FATAL_ERROR "GLFW submodules were not downloaded. GIT_SUBMODULE was turned off or failed")
endif()         

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/include/glm/CMakeLists.txt")
    message(FATAL_ERROR "GLM submodules were not downloaded. GIT_SUBMODULE was turned off or failed")
endif()         


add_subdirectory(include/GLFW)
add_subdirectory(include/GLM)

add_subdirectory(src/RHI)
add_subdirectory(src/BEngine)
add_subdirectory(src/VkCore)


target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    glm
    BEngine
    VkCore
)


#----------------------------[SHADER_COMPILATION]-----------------------------#  

set(SHADER_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/Shaders")
set(SHADER_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/Shaders")

file(MAKE_DIRECTORY "${SHADER_OUT_DIR}")

if(NOT DEFINED VULKAN_SDK_PATH)
    if(DEFINED ENV{VULKAN_SDK})
        set(VULKAN_SDK_PATH $ENV{VULKAN_SDK})
    else()
        message(FATAL_ERROR "VULKAN_SDK_PATH is not set! Set the Vulkan SDK environment variable.")
    endif()
endif()

if(WIN32)
    set(GLSLC "${VULKAN_SDK_PATH}/Bin/glslc.exe")
elseif(UNIX)
    set(GLSLC "$ENV{VULKAN_SDK_PATH}/bin/glslc")
endif()


file(GLOB_RECURSE SHADER_FILES
    "${SHADER_SRC_DIR}/*.vert"
    "${SHADER_SRC_DIR}/*.frag"
    "${SHADER_SRC_DIR}/*.comp"
)

set(COMPILED_SHADERS "")

foreach(SHADER ${SHADER_FILES})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    set(OUTPUT_SPV "${SHADER_OUT_DIR}/${SHADER_NAME}.spv")

    add_custom_command(
            OUTPUT "${OUTPUT_SPV}"
            COMMAND "${GLSLC}" "${SHADER}" -o "${OUTPUT_SPV}"
            DEPENDS "${SHADER}"
            COMMENT "Compiling shader ${SHADER_NAME}"
            VERBATIM
    )

    list(APPEND COMPILED_SHADERS "${OUTPUT_SPV}")
endforeach()

add_custom_target(compile_shaders ALL
    DEPENDS ${COMPILED_SHADERS}
    COMMENT "Building all Vulkan shaders"
)