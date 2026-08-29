
#include <vector>
#include <cassert>

#include "VertexSystem.hpp"

class IndexedShape
{
public:
    IndexedShape() = default;

    IndexedShape(DVS::VertexBuffer p_vertices, std::vector<unsigned short> p_indices)
    :
    m_vertices(std::move(p_vertices)), m_indices(std::move(p_indices))
    {
        assert(m_vertices.get_size_in_vertices() > 2);
        assert(m_indices.size() % 3 == 0);
    }

private:
    DVS::VertexBuffer m_vertices; 
    std::vector<unsigned short> m_indices;
};