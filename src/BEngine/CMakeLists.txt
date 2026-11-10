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

            Camera.hpp
            Camera.cpp

            Shapes/Cube.hpp
            Shapes/Square.hpp
            Shapes/Plane.hpp
            Shapes/Sphere.hpp

            ECS[oo]basic[oo]/Registry.hpp

            )
            
target_link_libraries(BEngine
    PRIVATE glfw
    PRIVATE glm
    PUBLIC RHI
)

target_include_directories(BEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
