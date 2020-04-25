#pragma once

#include <fstream>
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

    class shader_exception : public exception {
        using super = exception;
    public:
        shader_exception(): super() {}
        shader_exception(const char* message): super(message) {}
        shader_exception(const char* message, int code): super(message, code) {}
    };

    class shader_compile_exception : public shader_exception {
        using super = shader_exception;

        string message;

    public:
        shader_compile_exception(): message(), super() {}
        shader_compile_exception(std::string& mess): message(std::move(mess)), super(mess.c_str()) {}
        shader_compile_exception(const char* message): message(message), super(message) {}
        shader_compile_exception(const char* message, int code): message(message), super(message, code) {}

        const char* what() {
            return message.c_str();
        }
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
                throw shader_exception{ "Shader source file could not be opened" };

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

        friend class Program;

        string m_source;
        ShaderType m_type;
    public:GLuint m_shaderId;
    };
}
