add_library(BEngine STATIC
            Renderer.cpp)

target_include_directories(BEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})