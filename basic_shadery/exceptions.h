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
        string& get_message_internal() {
            return message;
        };

    public:
        shader_compile_exception(): message(), super() {}
        shader_compile_exception(std::string& mess): message(std::move(mess)), super(mess.c_str()) {}
        shader_compile_exception(const char* message): message(message), super(message) {}
        shader_compile_exception(const char* message, int code): message(message), super(message, code) {}

        const char* what() {
            return message.c_str();
        }
    };
}
