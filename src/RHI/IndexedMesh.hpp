#pragma once

#include <vector>
#include <cassert>

#include "VertexSystem.hpp"


class IndexedMesh
{
public:
    IndexedMesh() = delete;

    IndexedMesh(DVS::VertexBuffer p_vertices, std::vector<unsigned short> p_indices)
    :
    m_vertices(std::move(p_vertices)), m_indices(std::move(p_indices))
    {
        assert(m_vertices.get_size_in_vertices() > 2);
        assert(m_indices.size() % 3 == 0);
    }

public:
    DVS::VertexBuffer m_vertices; 
    std::vector<unsigned short> m_indices;
};