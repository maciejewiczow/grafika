#pragma once

#include <fstream>
#include <string>

#include <Gl/glew.h>

namespace gl
{
    using ifstream = std::basic_ifstream<GLchar, std::char_traits<GLchar>>;
    using string = std::basic_string<GLchar, std::char_traits<GLchar>>;
}
