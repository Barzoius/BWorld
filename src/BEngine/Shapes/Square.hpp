#include "IndexedShape.hpp"

class Square final
{
public:
    static IndexedShape make(DVS::VertexLayout p_layout)
    {
        using type = DVS::VertexLayout::ElementType;

        DVS::VertexBuffer vertices{std::move(p_layout), 4u};
        vertices[0].attribute<type::Position2D>() = {-0.5f, -0.5f};
        vertices[1].attribute<type::Position2D>() = {0.5f, -0.5f};
        vertices[2].attribute<type::Position2D>() = {0.5f, 0.5f};
        vertices[3].attribute<type::Position2D>() = {-0.5f, 0.5f};

        return {std::move(vertices), {0, 1, 2, 2, 3, 0}};

    }
};
