#include "VertexSystem.hpp"


class Model
{
public:
    Model() = default;
    Model(DVS::VertexBuffer, std::vector<unsigned short>);

    void set_scale();
    void set_position();
    void set_rotation();

private:
    DVS::VertexBuffer m_vertices;
    std::vector<unsigned short> m_indices;
};