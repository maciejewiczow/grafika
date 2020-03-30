#include "Program.h"

gl::Program& gl::Program::link() {
    glLinkProgram(m_programId);

    GLint hasLinked;
    glGetProgramiv(m_programId, GL_COMPILE_STATUS, &hasLinked);

    if (hasLinked)
        return *this;

    GLint log_length;
    glGetProgramiv(m_programId, GL_INFO_LOG_LENGTH, &log_length);

    gl::string buffer;
    buffer.resize(static_cast<std::size_t>(log_length)+1);

    glGetProgramInfoLog(m_programId, log_length, &log_length, buffer.data());

    throw program_link_exception{ buffer };
};
