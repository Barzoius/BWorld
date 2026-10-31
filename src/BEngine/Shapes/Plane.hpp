#include "IndexedShape.hpp"

class Plane final
{
public:
    static IndexedShape make(DVS::VertexLayout p_layout)
    {
        using type = DVS::VertexLayout::ElementType;

        DVS::VertexBuffer vertices{std::move(p_layout), 4u};


        return {std::move(vertices), {}};

    }
};
