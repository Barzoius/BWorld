#pragma once

#include "IndexedMesh.hpp"
#include "Utils/Utils.hpp"

class Sphere final
{
public:
    static IndexedMesh make()
    {
        return make_uv_sphere();
    }

    static IndexedMesh make_uv_sphere(int stacks = 16, int sectors = 32)
    {
        using type = DVS::VertexLayout::ElementType;

        assert(stacks >= 2);
        assert(sectors >= 3);

        constexpr float radius = 1.0f;
        constexpr float pi = 3.14159265358979323846f;

        const float latitude_step = pi / stacks;
        const float longitude_step = 2.0f * pi / sectors;

        const size_t vertex_count =
            static_cast<size_t>(stacks + 1) * (sectors + 1);

        
        assert(vertex_count <=
            static_cast<size_t>(
                std::numeric_limits<unsigned short>::max()
            ) + 1);

        DVS::VertexBuffer vertices{
            DVS::get_universal_layout(), vertex_count
        };

        
        for (int i = 0; i <= stacks; i++)
        {
            const float stack_angle = pi / 2.0f - i * latitude_step;

            const float z = radius * sinf(stack_angle);
            const float xy = radius * cosf(stack_angle);

            for (int j = 0; j <= sectors; ++j)
            {
                const float sector_angle = j * longitude_step;

                const float x = xy * cosf(sector_angle);
                const float y = xy * sinf(sector_angle);

                const size_t index =
                    static_cast<size_t>(i) * (sectors + 1) + j;

                vertices[index].attribute<type::Position3D>() =
                {
                    x, y, z
                };

                glm::vec3 c = bengine_rand::rand_normalized_color();
                vertices[index].attribute<type::Float3Color>() = {c.x, c.y, c.z};
            }
        }

        std::vector<unsigned short> indices;
        indices.reserve(
            static_cast<size_t>(6) * sectors * (stacks - 1)
        );

        for (int i = 0; i < stacks; i++)
        {
            size_t k1 = static_cast<size_t>(i) * (sectors + 1);
            size_t k2 = k1 + sectors + 1;

            for (int j = 0; j < sectors; ++j, ++k1, ++k2)
            {
                if (i != 0)
                {
                    indices.push_back(
                        static_cast<unsigned short>(k1)
                    );
                    indices.push_back(
                        static_cast<unsigned short>(k2)
                    );
                    indices.push_back(
                        static_cast<unsigned short>(k1 + 1)
                    );
                }

                if (i != stacks - 1)
                {
                    indices.push_back(
                        static_cast<unsigned short>(k1 + 1)
                    );
                    indices.push_back(
                        static_cast<unsigned short>(k2)
                    );
                    indices.push_back(
                        static_cast<unsigned short>(k2 + 1)
                    );
                }
            }
        }

        return
        {
            std::move(vertices),
            std::move(indices)
        };
    }
};