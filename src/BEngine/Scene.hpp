#pragma once
#include "Window.hpp"

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