#pragma once

#include "Camera.h"

namespace gl
{
    template<typename>
    class Uniform;

    class CameraControls {
    public:
        CameraControls(): m_view_unif(nullptr), m_projection_unif(nullptr) {}

        virtual void onUpdate() = 0;
        void setViewUniform(gl::Uniform<glm::mat4>& unif) { m_view_unif = &unif; }
        void setProjectionUniform(gl::Uniform<glm::mat4>& unif) { m_projection_unif = &unif; }

    protected:
        gl::Uniform<glm::mat4>* m_view_unif;
        gl::Uniform<glm::mat4>* m_projection_unif;
    };
}
