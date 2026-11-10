#pragma once

#include "Window.hpp"
#include "VertexSystem.hpp"
#include "Camera.hpp"
#include "Timer.hpp"
#include "RenderObject.hpp"


#include <memory>

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
    Camera  camera;
    Timer   timer;

    std::vector<RenderObject> objects;

};