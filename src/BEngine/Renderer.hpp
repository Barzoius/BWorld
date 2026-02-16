
class Renderer
{
public:
    virtual void Initialize(void* windowHandle) = 0;
    virtual void RenderFrame() = 0;
    virtual void Shutdown() = 0;

};