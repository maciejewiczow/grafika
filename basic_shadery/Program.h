#pragma once

#include <GL/glew.h>

#include "Shader.h"
#include "exceptions.h"

namespace gl
{
    template<typename T>
    class Uniform;

    class program_link_exception : public exception {
        using super = exception;
        string message;

    public:
        program_link_exception(): message(), super() {}
        program_link_exception(std::string& mess): message(std::move(mess)), super(mess.c_str()) {}
        program_link_exception(const char* message): message(message), super(message) {}
        program_link_exception(const char* message, int code): message(message), super(message, code) {}

        const char* what() {
            return message.c_str();
        }
    };

    class Program {
    public:
        Program(): m_programId(glCreateProgram()) {};

        Program(const Program&) = delete;
        Program& operator=(const Program&) = delete;

        Program(Program&& other) noexcept: m_programId(0) {
            std::swap(m_programId, other.m_programId);
        }
        Program& operator=(Program&& other) noexcept {
            if (this != &other) {
                std::swap(m_programId, other.m_programId);
            }
            return *this;
        }

        Program& bindFragDataLocation(GLuint colorNr, const GLchar* name) {
            glBindFragDataLocation(m_programId, colorNr, name);
            return *this;
        };

        Program& useShader(const Shader& shader) {
            glAttachShader(m_programId, shader.m_shaderId);
            return *this;
        };

        Program& link();

        Program& bind() {
            glUseProgram(m_programId);
            return *this;
        };

        GLint getAttributeLocation(const GLchar* name) {
            return glGetAttribLocation(m_programId, name);
        }

        ~Program() {
            glDeleteProgram(m_programId);
        };

        template<typename T>
        inline Uniform<T> createUniform(const char* name) {
            return Uniform<T>{*this, name};
        };

        template<typename T>
        inline Uniform<T> createUniform(const char* name, const T& val) {
            return Uniform<T>{*this, name, val};
        };

    private:
        GLuint m_programId;

        template<typename>
        friend class Uniform;
    };
}
