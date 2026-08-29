#include "Model.hpp"

class VkModel : public Model
{
public:
    VkModel(DVS::VertexBuffer, std::vector<unsigned short>);

    void update();
};