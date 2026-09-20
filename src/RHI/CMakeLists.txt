# add_library(RHI INTERFACE)

# target_include_directories(RHI INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_library(RHI STATIC
    Context.hpp
    Model.hpp
    Renderer.hpp
    ShaderTypes.hpp
    SurfaceInfo.hpp
    UniformSystem.cpp
    UniformSystem.hpp
    VertexSystem.hpp
)

target_include_directories(RHI PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)