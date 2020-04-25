#pragma once

#include <stdexcept>

#include "core.h"

namespace gl
{
    class exception : public std::exception {
        using super = std::exception;
    public:
        exception(): super() {}
        exception(const char* message): super(message) {}
        exception(const char* message, int code): super(message, code) {}
    };
}
