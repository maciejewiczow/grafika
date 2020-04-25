#pragma once

#include <GL/glew.h>
#include <glm/matrix.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Program.h"

namespace gl
{
    class Program;

    template<typename T>
    class Uniform {
        friend class Program;

        Uniform(const Program& prog, const char* name): m_prog(&prog), m_value(), m_uniformId(0) {
            m_uniformId = glGetUniformLocation(m_prog->m_programId, name);
        }

        Uniform(const Program& prog, const char* name, const T& value): m_prog(&prog), m_value(value), m_uniformId(0) {
            m_uniformId = glGetUniformLocation(m_prog->m_programId, name);
            update();
        }

    public:
        Uniform(const Uniform&) = delete;
        Uniform& operator=(const Uniform&) = delete;

        Uniform(Uniform&& other): m_prog(other.m_prog), m_value(std::move(other.m_value)), m_uniformId(other.m_uniformId) {
            other.m_uniformId = 0;
            other.m_prog = nullptr;
        };
        Uniform& operator=(Uniform& other) noexcept {
            if (this != &other) {
                std::swap(m_uniformId, other.m_uniformId);
                std::swap(m_value, other.m_value);
                std::swap(m_prog, other.m_prog);
            }
            return *this;
        };

        Uniform& operator=(const T& val) {
            m_value = val;
            update();
            return *this;
        };
        Uniform& operator+=(const T& val) {
            m_value += val;
            update();
            return *this;
        };
        Uniform& operator-=(const T& val) {
            m_value -= val;
            update();
            return *this;
        };
        Uniform& operator*=(const T& val) {
            m_value *= val;
            update();
            return *this;
        };
        Uniform& operator/=(const T& val) {
            m_value /= val;
            update();
            return *this;
        };

        const T& value() const {
            return m_value;
        }

        void update();
    private:

        T m_value;
        GLint m_uniformId;
        const Program* m_prog;
    };
}
