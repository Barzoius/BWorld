#pragma once

#include "IndexedMesh.hpp"

class Plane final
{
public:
    static IndexedMesh make()
    {
        using type = DVS::VertexLayout::ElementType;

        DVS::VertexBuffer vertices{DVS::get_universal_layout(), 4u};


        return {std::move(vertices), {}};

    }
};
