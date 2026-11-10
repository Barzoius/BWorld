#pragma once

#include "IndexedMesh.hpp"

class Square final
{
public:
    static IndexedMesh make()
    {
        using type = DVS::VertexLayout::ElementType;

        DVS::VertexBuffer vertices{DVS::get_universal_layout(), 4u};
        vertices[0].attribute<type::Position3D>() = {-0.5f, -0.5, 0.0f};
        vertices[1].attribute<type::Position3D>() = {0.5f, -0.5f, 0.0f};
        vertices[2].attribute<type::Position3D>() = {0.5f, 0.5f, 0.0f};
        vertices[3].attribute<type::Position3D>() = {-0.5f, 0.5f, 0.0f};

        return {std::move(vertices), {0, 1, 2, 2, 3, 0}};

    }

        
    static IndexedMesh make_colored()
    {
        using type = DVS::VertexLayout::ElementType;

        DVS::VertexBuffer vertices{DVS::get_universal_layout(), 4u};
        vertices[0].attribute<type::Position3D>() = {-0.5f, -0.5, 0.0f};
        vertices[1].attribute<type::Position3D>() = {0.5f, -0.5f, 0.0f};
        vertices[2].attribute<type::Position3D>() = {0.5f, 0.5f, 0.0f};
        vertices[3].attribute<type::Position3D>() = {-0.5f, 0.5f, 0.0f};

        vertices[0].attribute<type::Float3Color>() = {1.0f, 1.0, 1.0f};
        vertices[1].attribute<type::Float3Color>() = {0.0f, 1.0f, 0.0f};
        vertices[2].attribute<type::Float3Color>() = {0.0f, 0.0f, 1.0f};
        vertices[3].attribute<type::Float3Color>() = {0.0f, 1.0f, 1.0f};

        return {std::move(vertices), {0, 1, 2, 2, 3, 0}};

    }
};
