add_library(BEngine STATIC
            Engine.cpp
            Engine.hpp
            
            Window.cpp
            Window.hpp
            
            Scene.cpp
            Scene.hpp
            )
            
target_link_libraries(BEngine
    PRIVATE glfw
    PUBLIC RHI
)

target_include_directories(BEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
