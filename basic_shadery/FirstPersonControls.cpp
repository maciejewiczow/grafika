#include "FirstPersonControls.h"

void gl::FirstPersonControls::onUpdate() {
    updatePosition();
    updateDirection();
}

void gl::FirstPersonControls::lookAt(const glm::vec3& pos) {
    m_camera.lookAt(pos);
    if (m_view_unif)
        *m_view_unif = m_camera.m_view;
}

void gl::FirstPersonControls::captureMouse() {
    m_viewport.setMouseCursorVisible(false);
    m_isMouseCaptured = true;
    centerMouse();
}

void gl::FirstPersonControls::releaseMouse() {
    m_viewport.setMouseCursorVisible(true);
    m_isMouseCaptured = false;
}

void gl::FirstPersonControls::toggleMouseCapture() {
    if (m_isMouseCaptured)
        releaseMouse();
    else
        captureMouse();
}

void gl::FirstPersonControls::centerMouse() {
    auto size = m_viewport.getSize();
    size /= 2u;
    sf::Mouse::setPosition(static_cast<sf::Vector2i>(size), m_viewport);
}

inline void gl::FirstPersonControls::updatePosition() {
    // ignore all keyboard events when window is not focused
    if (m_viewport.getSystemHandle() != GetFocus())
        return;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        m_camera.m_position.x += m_camera.m_direction.x * moveSpeed;
        m_camera.m_position.z += m_camera.m_direction.z * moveSpeed;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        m_camera.m_position.x -= m_camera.m_direction.x * moveSpeed;
        m_camera.m_position.z -= m_camera.m_direction.z * moveSpeed;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        m_camera.m_position -= glm::cross(m_camera.m_direction, m_camera.m_up) * moveSpeed;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        m_camera.m_position += glm::cross(m_camera.m_direction, m_camera.m_up) * moveSpeed;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::RShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
        m_camera.m_position -= m_camera.m_up * moveSpeed;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        m_camera.m_position += m_camera.m_up * moveSpeed;
    }

    m_camera.updateViewMatrix();
    if (m_view_unif)
        *m_view_unif = m_camera.m_view;
}

void gl::FirstPersonControls::updateDirection() {
    if (!m_isMouseCaptured)
        return;

    auto mousePos = sf::Mouse::getPosition(m_viewport);
    auto center = static_cast<sf::Vector2i>(m_viewport.getSize()) / 2;

    int xoffset = mousePos.x - center.x;
    int yoffset = center.y - mousePos.y; // reversed since y-coordinates go from bottom to top

    if (xoffset == 0 || yoffset == 0)
        return;

    centerMouse();

    m_yaw += ((float) xoffset) * lookSpeed;
    m_pitch = glm::clamp(m_pitch + ((float) yoffset) * lookSpeed, -89.f, 89.f);

    m_camera.m_direction.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_camera.m_direction.y = sin(glm::radians(m_pitch));
    m_camera.m_direction.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

    m_camera.updateViewMatrix();
    if (m_view_unif)
        *m_view_unif = m_camera.m_view;
}
