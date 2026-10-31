#pragma once

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Camera
{
public:
    Camera();

    void reset()               noexcept;
    void translate(glm::vec3)  noexcept;
    void rotate(float, float)  noexcept;

    glm::vec3 get_pos()  const noexcept;
    glm::mat4 get_view() const noexcept;
private:
    glm::vec3 m_pos;
    glm::vec3 m_target;
    glm::vec3 m_direction;
    
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;

    glm::mat4 m_view;

    float m_pitch;
    float m_yaw;

    static constexpr float travelSpeed   = 50.0f;
    static constexpr float rotationSpeed = 0.004f;
};