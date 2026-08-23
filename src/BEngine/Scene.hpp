#pragma once
#include "Window.hpp"
#include "VertexSystem.hpp"

class Scene
{
public:
    Scene();
    ~Scene();
    

    // virtual void RenderMesh();
    // virtual void RenderMeshes();

    void Run();
    void Exit();
    void Update();

private:
    Window* window;
};