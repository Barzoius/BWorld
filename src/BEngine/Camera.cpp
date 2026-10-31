#include "Camera.hpp"

Camera::Camera() { reset(); }

void Camera::reset() noexcept
{
    m_pos       = { 0.0f, 0.0f, 3.0f };
    m_target    = { 0.0f, 0.0f, 0.0f }; //center of the space
    m_up        = { 0.0f, 1.0f, 0.0f };

    glm::vec3 up= { 0.0f, 1.0f, 0.0f };
    m_direction = glm::normalize(m_pos - m_target);
    m_right     = glm::normalize(glm::cross(m_direction, up));
    m_up        = glm::cross(m_direction, m_right);

    m_pitch     = 0.0f;
    m_yaw       = 0.45f;

    m_view = glm::lookAt( m_pos, m_target, m_up );
}

void Camera::translate(glm::vec3 translation) noexcept
{
    m_pos += m_right * translation.x;
    m_pos += m_up    * translation.y;
    m_pos += m_direction * translation.z;

    m_target = m_pos - m_direction;

    m_view = glm::lookAt(m_pos, m_target, m_up);
}


void Camera::rotate(float dx, float dy) noexcept
{

}

glm::vec3 Camera::get_pos() const noexcept { return m_pos; }

glm::mat4 Camera::get_view() const noexcept
{
    return m_view;
}

