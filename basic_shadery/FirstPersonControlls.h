﻿#pragma once
#include <cmath>

#include <SFML/Window.hpp>

#include "CameraControlls.h"
#include "PerspectiveCamera.h"
#include "Uniform.h"

#include <Windows.h>
// because windows contains stupid defines
#undef near
#undef far

namespace gl
{
    class FirstPersonControlls : public CameraControlls {
        using super = CameraControlls;
    public:
        FirstPersonControlls(PerspectiveCamera& cam, sf::Window& win):
            m_camera(cam),
            m_viewport(win),
            m_yaw(atan2(cam.m_direction.x, cam.m_direction.z)),
            m_pitch(atan2(cam.m_direction.y, glm::length(cam.m_direction)))
        {
            captureMouse();
        }

        virtual void onUpdate() override;

        void lookAt(const glm::vec3& pos);

        void captureMouse();
        void releaseMouse();
        void toggleMouseCapture();
        bool isMouseCaptured() const { return m_isMouseCaptured; }

        float moveSpeed = 0.1f;
        float lookSpeed = 0.15f;

    private:
        inline void centerMouse();
        inline void updatePosition();
        inline void updateDirection();

        PerspectiveCamera& m_camera;
        sf::Window& m_viewport;

        float m_yaw, m_pitch;
        bool m_isMouseCaptured;
    };
}
