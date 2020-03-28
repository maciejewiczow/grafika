#pragma once

#include <fstream>
#include <filesystem>
#include <string>
#include <iterator>

#include "core.h"
#include "exceptions.h"

#include <GL/glew.h>

namespace gl
{
    enum class ShaderType {
        Vertex = GL_VERTEX_SHADER,
        Geometry = GL_GEOMETRY_SHADER,
        Fragment = GL_FRAGMENT_SHADER
    };

    class Shader {
    public:
        static Shader fromSource(const GLchar* m_source, ShaderType m_type) {
            return Shader{ m_source, m_type };
        }

        static Shader fromSource(const string& m_source, ShaderType m_type) {
            return Shader{ m_source, m_type };
        }

        static Shader fromFile(const char* filename, ShaderType m_type) {
            ifstream in{ filename };

            if (!in)
                throw shader_exception{ "Source file could not be opened" };

            return Shader{ in, m_type };
        }

        Shader(): m_type(), m_shaderId(0), m_source() {}

        // non-copyable
        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;

        Shader(Shader&&) noexcept;
        Shader& operator=(Shader&&) noexcept;

        ShaderType get_type() const { return m_type; };
        void compile() const;

        ~Shader();

    private:
        Shader(const GLchar* m_source, ShaderType);
        Shader(const string& m_source, ShaderType);
        Shader(ifstream& m_source, ShaderType);
        inline void initShader();

        string m_source;
        ShaderType m_type;
    public:GLuint m_shaderId;
    };
}
