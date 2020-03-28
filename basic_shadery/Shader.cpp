#include "Shader.h"

using namespace gl;

Shader::Shader(const GLchar* m_source, ShaderType m_type):
    m_type(m_type),
    m_source(m_source),
    m_shaderId(0)
{
    initShader();
}

Shader::Shader(const string& m_source, ShaderType m_type):
    m_type(m_type),
    m_source(m_source),
    m_shaderId(0)
{
    initShader();
}

Shader::Shader(ifstream& m_source, ShaderType m_type):
    m_type(m_type),
    m_source(std::istreambuf_iterator<GLchar>{m_source}, {})
{
    initShader();
}

void Shader::initShader() {
    m_shaderId = glCreateShader((GLenum) m_type);

    if (m_shaderId == 0)
        throw shader_exception("Shader creation failed");

    auto data = m_source.c_str();
    glShaderSource(m_shaderId, 1, &data, NULL);
}

Shader::Shader(Shader&& other) noexcept:
    m_type(other.m_type),
    m_shaderId(other.m_shaderId),
    m_source(std::move(other.m_source))
{
    other.m_shaderId = 0;
}

Shader& Shader::operator=(Shader&& rhs) noexcept {
    if (&rhs != this) {
        std::swap(m_shaderId, rhs.m_shaderId);
        std::swap(m_source, rhs.m_source);
        m_type = rhs.m_type;
    }
    return *this;
}

void Shader::compile() const {
    glCompileShader(m_shaderId);

    GLint hasCompiled;
    glGetShaderiv(m_shaderId, GL_COMPILE_STATUS, &hasCompiled);

    if (hasCompiled)
        return;

    GLint log_length;
    glGetShaderiv(m_shaderId, GL_INFO_LOG_LENGTH, &log_length);

    gl::string buffer;
    buffer.resize(static_cast<std::size_t>(log_length)+1);

    glGetShaderInfoLog(m_shaderId, log_length, &log_length, buffer.data());

    throw shader_compile_exception{ buffer };
}

Shader::~Shader() {
    glDeleteShader(m_shaderId);
}
