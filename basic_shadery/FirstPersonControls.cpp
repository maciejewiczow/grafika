#include "FirstPersonControls.h"

void gl::FirstPersonControls::update(float timeStep) {
    // do not update controls when window is in background
    if (!m_viewport.hasFocus()) return;

    updatePosition(timeStep);
    updateDirection(timeStep);
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

inline void gl::FirstPersonControls::updatePosition(float timeStep) {
    float timeMoveSpeed = moveSpeed * timeStep;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        m_camera.m_position += glm::normalize(glm::vec3{ m_camera.m_direction.x, .0f,  m_camera.m_direction.z }) * timeMoveSpeed;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        m_camera.m_position -= glm::normalize(glm::vec3{ m_camera.m_direction.x, .0f,  m_camera.m_direction.z }) * timeMoveSpeed;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        m_camera.m_position -= glm::normalize(glm::cross(m_camera.m_direction, m_camera.m_up)) * timeMoveSpeed;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        m_camera.m_position += glm::normalize(glm::cross(m_camera.m_direction, m_camera.m_up)) * timeMoveSpeed;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::RShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
        m_camera.m_position -= m_camera.m_up * timeMoveSpeed;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        m_camera.m_position += m_camera.m_up * timeMoveSpeed;
    }

    m_camera.updateViewMatrix();
    if (m_view_unif)
        *m_view_unif = m_camera.m_view;
}

void gl::FirstPersonControls::updateDirection(float timeStep) {
    if (!m_isMouseCaptured)
        return;

    auto mousePos = sf::Mouse::getPosition(m_viewport);
    auto center = static_cast<sf::Vector2i>(m_viewport.getSize()) / 2;

    int xoffset = mousePos.x - center.x;
    int yoffset = center.y - mousePos.y; // reversed since y-coordinates go from bottom to top

    if (xoffset == 0 || yoffset == 0)
        return;

    centerMouse();

    m_yaw += static_cast<float>(xoffset) * lookSpeed * timeStep;
    m_pitch = glm::clamp(m_pitch + static_cast<float>(yoffset) * lookSpeed * timeStep, -89.f, 89.f);

    m_camera.m_direction.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_camera.m_direction.y = sin(glm::radians(m_pitch));
    m_camera.m_direction.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

    m_camera.updateViewMatrix();
    if (m_view_unif)
        *m_view_unif = m_camera.m_view;
}
