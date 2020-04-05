#pragma once

#include <glm/matrix.hpp>
#include <glm/ext.hpp>

namespace gl
{
    class Camera {
    public:
        const glm::vec3& getPosition() const { return m_position; };
        const glm::mat4& getProjectionMatrix() const { return m_projection; };
        const glm::mat4& getViewMatrix() const { return m_view; };

        void setPosition(const glm::vec3& pos) {
            m_position = pos;
            updateViewMatrix();
        };

    protected:
        virtual void updateViewMatrix() = 0;

        Camera():
            m_position{ .0f, .0f, .0f },
            m_view{ glm::mat4{1.f} },
            m_projection{ glm::mat4{1.f} }
        {}

        glm::vec3 m_position;

        glm::mat4 m_view;
        glm::mat4 m_projection;
    };
}
