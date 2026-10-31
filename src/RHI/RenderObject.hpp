#pragma once

#include "VertexSystem.hpp"
#include <vector>
#include <string>
#include <memory>

// use indexed shape instead
struct Mesh
{
    DVS::VertexBuffer     vertex_buffer;
    std::vector<uint16_t> index_buffer;
};

struct Material
{

};

struct RenderObjec
{
    int                       id;
    std::unique_ptr<Mesh>     mesh;
    std::unique_ptr<Material> material;
    //transform

};