#pragma once
#include <cmath>

#include <SFML/Window.hpp>

#include "CameraControls.h"
#include "PerspectiveCamera.h"
#include "Uniform.h"

namespace gl
{
    class FirstPersonControls : public CameraControls {
        using super = CameraControls;

    public:
        FirstPersonControls(PerspectiveCamera& cam, sf::Window& win):
            m_camera(cam),
            m_viewport(win),
            m_yaw(atan2(cam.m_direction.x, cam.m_direction.z)),
            m_pitch(atan2(cam.m_direction.y, glm::length(cam.m_direction)))
        {
            captureMouse();
        }

        virtual void onUpdate(float timeStep) override;

        void lookAt(const glm::vec3& pos);

        void captureMouse();
        void releaseMouse();
        void toggleMouseCapture();
        bool isMouseCaptured() const { return m_isMouseCaptured; }

        float moveSpeed = 0.000015f;
        float lookSpeed = 0.000015f;

    private:
        inline void centerMouse();
        inline void updatePosition(float timeStep);
        inline void updateDirection(float timeStep);

        PerspectiveCamera& m_camera;
        sf::Window& m_viewport;

        float m_yaw, m_pitch;
        bool m_isMouseCaptured;
    };
} // namespace gl
