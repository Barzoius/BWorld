#include "IndexedShape.hpp"

class Cube final
{
public:
    static IndexedShape make(DVS::VertexLayout p_layout)
    {
        using type = DVS::VertexLayout::ElementType;

        constexpr float side = 1.0f / 2.0f;

        DVS::VertexBuffer vertices{std::move(p_layout), 8u };

        vertices[0].attribute<type::Position3D>() = { -side, -side, -side };
        vertices[1].attribute<type::Position3D>() = {  side, -side, -side };
        vertices[2].attribute<type::Position3D>() = { -side,  side, -side };
        vertices[3].attribute<type::Position3D>() = {  side,  side, -side };
        vertices[4].attribute<type::Position3D>() = { -side, -side,  side };
        vertices[5].attribute<type::Position3D>() = {  side, -side,  side };
        vertices[6].attribute<type::Position3D>() = { -side,  side,  side };
        vertices[7].attribute<type::Position3D>() = {  side,  side,  side };

        return
        {
            std::move(vertices),
            {
                0,2,1, 2,3,1,
                1,3,5, 3,7,5,
                2,6,3, 3,6,7,
                4,5,7, 4,7,6,
                0,4,2, 2,4,6,
                0,1,4, 1,5,4
            }
        };

    }
};