add_library(BEngine STATIC
            Engine.cpp
            Engine.hpp
            
            Window.cpp
            Window.hpp
            
            Scene.cpp
            Scene.hpp

            Inputs/Keyboard.hpp
            Inputs/Keyboard.cpp

            Inputs/Mouse.hpp
            Inputs/Mouse.cpp

            Utils/Utils.hpp
            Utils/Utils.cpp


            )
            
target_link_libraries(BEngine
    PRIVATE glfw
    PRIVATE glm
    PUBLIC RHI
)

target_include_directories(BEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
