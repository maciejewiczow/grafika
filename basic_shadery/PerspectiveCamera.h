#pragma once
#include "Camera.h"

namespace gl
{
    class FirstPersonControlls;

    class PerspectiveCamera : public Camera {
        using super = Camera;
    public:
        PerspectiveCamera(float fov, float aspect, float near, float far):
            super(),
            m_fov(fov),
            m_aspect(aspect),
            m_near(near),
            m_far(far)
        {
            updateProjectionMatrix();
            updateViewMatrix();
        }

        PerspectiveCamera(float fov, const glm::tvec2<unsigned>& res, float near, float far):
            super(),
            m_fov(fov),
            m_aspect((float) res.x/res.y),
            m_near(near),
            m_far(far)
        {
            updateProjectionMatrix();
            updateViewMatrix();
        }

        const glm::vec3& getDirection() const { return m_direction; }
        void setDirection(const glm::vec3& dir) {
            m_direction = dir;
            updateViewMatrix();
        };

        void setFov(float fov) {
            m_fov = fov;
            updateProjectionMatrix();
        }

        void setAspect(float aspect) {
            m_aspect = aspect;
            updateProjectionMatrix();
        }
        inline void setAspect(const glm::tvec2<unsigned>& res) {
            setAspect((float) res.x/res.y);
        }

        void lookAt(const glm::vec3& target) {
            m_direction = glm::normalize(target - m_position);
            updateViewMatrix();
        }

        void setProjection(float fov, float aspect, float near, float far) {
            m_fov = fov;
            m_near = near;
            m_far = far;
            m_aspect = aspect;
            updateProjectionMatrix();
        }
        inline void setProjection(float fov, const glm::tvec2<unsigned>& res, float near, float far) {
            setProjection(fov, (float) res.x/res.y, near, far);
        }

    private:
        virtual void updateViewMatrix() override {
            m_view = glm::lookAt(m_position, m_position + m_direction, m_up);
        };

        virtual void updateProjectionMatrix() override {
            m_projection = glm::perspective(m_fov, m_aspect, m_near, m_far);
        }

        float m_fov, m_aspect, m_near, m_far;
        glm::vec3 m_direction{ .0f, .0f, -1.0f };
        glm::vec3 m_up{ .0f, 1.0f, .0f };

        friend class FirstPersonControlls;
    };
}
