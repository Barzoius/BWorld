
#include <vector>
#include <cassert>

class IndexedShape
{
public:
    IndexedShape() = default;

    IndexedShape(bool p_vertices, std::vector<unsigned short> p_indices)
    :
    m_vertices(std::move(p_vertices)), m_indices(std::move(p_indices))
    {
        //assert(m_vertices.size() > 2);
        assert(m_indices.size() % 3 == 0);
    }

private:
    bool m_vertices; // to be repalced
    std::vector<unsigned short> m_indices;
};