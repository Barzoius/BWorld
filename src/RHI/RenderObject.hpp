#pragma once

#include "IndexedMesh.hpp"
#include <vector>
#include <string>
#include <memory>


struct Material
{

};

struct model
{
    int x,y,z,w;
};


struct RenderObject
{
    int          id;
    IndexedMesh* mesh;
    Material     material;
    model        model;
};