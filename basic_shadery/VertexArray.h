#pragma once

#include <GL/glew.h>

namespace gl
{
    class VertexArray {
    public:
        VertexArray() {
            glGenVertexArrays(1, &m_arrayId);
        };

        void bind() const {
            glBindVertexArray(m_arrayId);
        };

        ~VertexArray() {
            glDeleteVertexArrays(1, &m_arrayId);
        };

    private:
        GLuint m_arrayId;
    };
}
