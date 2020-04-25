#include "Uniform.h"

namespace gl
{
    // Floats
    void Uniform<GLfloat>::update() {
        glProgramUniform1f(m_prog->m_programId, m_uniformId, m_value);
    }

    void Uniform<glm::tvec2<GLfloat>>::update() {
        glProgramUniform2f(m_prog->m_programId, m_uniformId, m_value.x, m_value.y);
    }

    void Uniform<glm::tvec3<GLfloat>>::update() {
        glProgramUniform3f(m_prog->m_programId, m_uniformId, m_value.x, m_value.y, m_value.z);
    }

    void Uniform<glm::tvec4<GLfloat>>::update() {
        glProgramUniform4f(m_prog->m_programId, m_uniformId, m_value.x, m_value.y, m_value.z, m_value.w);
    }

    // Ints
    void Uniform<GLint>::update() {
        glProgramUniform1i(m_prog->m_programId, m_uniformId, m_value);
    }

    void Uniform<glm::tvec2<GLint>>::update() {
        glProgramUniform2i(m_prog->m_programId, m_uniformId, m_value.x, m_value.y);
    }

    void Uniform<glm::tvec3<GLint>>::update() {
        glProgramUniform3i(m_prog->m_programId, m_uniformId, m_value.x, m_value.y, m_value.z);
    }

    void Uniform<glm::tvec4<GLint>>::update() {
        glProgramUniform4i(m_prog->m_programId, m_uniformId, m_value.x, m_value.y, m_value.z, m_value.w);
    }

    // uints
    void Uniform<GLuint>::update() {
        glProgramUniform1ui(m_prog->m_programId, m_uniformId, m_value);
    }

    void Uniform<glm::tvec2<GLuint>>::update() {
        glProgramUniform2ui(m_prog->m_programId, m_uniformId, m_value.x, m_value.y);
    }

    void Uniform<glm::tvec3<GLuint>>::update() {
        glProgramUniform3ui(m_prog->m_programId, m_uniformId, m_value.x, m_value.y, m_value.z);
    }

    void Uniform<glm::tvec4<GLuint>>::update() {
        glProgramUniform4ui(m_prog->m_programId, m_uniformId, m_value.x, m_value.y, m_value.z, m_value.w);
    }

    // matrices
    void Uniform<glm::mat2>::update() {
        glProgramUniformMatrix2fv(m_prog->m_programId, m_uniformId, 1, GL_FALSE, glm::value_ptr(m_value));
    }

    void Uniform<glm::mat2x3>::update() {
        glProgramUniformMatrix2x3fv(m_prog->m_programId, m_uniformId, 1, GL_FALSE, glm::value_ptr(m_value));
    }

    void Uniform<glm::mat2x4>::update() {
        glProgramUniformMatrix2x4fv(m_prog->m_programId, m_uniformId, 1, GL_FALSE, glm::value_ptr(m_value));
    }

    void Uniform<glm::mat3>::update() {
        glProgramUniformMatrix3fv(m_prog->m_programId, m_uniformId, 1, GL_FALSE, glm::value_ptr(m_value));
    }

    void Uniform<glm::mat3x2>::update() {
        glProgramUniformMatrix3x2fv(m_prog->m_programId, m_uniformId, 1, GL_FALSE, glm::value_ptr(m_value));
    }

    void Uniform<glm::mat3x4>::update() {
        glProgramUniformMatrix3x4fv(m_prog->m_programId, m_uniformId, 1, GL_FALSE, glm::value_ptr(m_value));
    }

    void Uniform<glm::mat4>::update() {
        glProgramUniformMatrix4fv(m_prog->m_programId, m_uniformId, 1, GL_FALSE, glm::value_ptr(m_value));
    }

    void Uniform<glm::mat4x2>::update() {
        glProgramUniformMatrix4x2fv(m_prog->m_programId, m_uniformId, 1, GL_FALSE, glm::value_ptr(m_value));
    }

    void Uniform<glm::mat4x3>::update() {
        glProgramUniformMatrix4x3fv(m_prog->m_programId, m_uniformId, 1, GL_FALSE, glm::value_ptr(m_value));
    }
}
