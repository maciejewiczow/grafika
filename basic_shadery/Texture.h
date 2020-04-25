#pragma once

#include <GL/glew.h>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include <STB/stb_image.h>

#include "exceptions.h"

namespace gl
{
    class image_load_exception : public gl::exception {
        using super = gl::exception;
    public:
        image_load_exception(): super() {}
        image_load_exception(const char* message): super(message) {}
        image_load_exception(const char* message, int code): super(message, code) {}
    };

    class Texture {
    public:
        enum class Wrap {
            Repeat = GL_REPEAT,
            Clamp = GL_CLAMP
        };

        enum class MinFilter {
            Nearest = GL_NEAREST,
            Linear = GL_LINEAR,
            Nearest_MipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
            Nearest_MipmapLinear = GL_NEAREST_MIPMAP_LINEAR,
            Linear_MipmapNearest = GL_LINEAR_MIPMAP_NEAREST,
            Linear_MipmapLinear = GL_LINEAR_MIPMAP_LINEAR
        };

        enum class MagFilter {
            Nearest = GL_NEAREST,
            Linear = GL_LINEAR
        };

        Texture():
            m_texId(0),
            width(0),
            height(0),
            channels(0),
            data(nullptr)
        {
            glGenTextures(1, &m_texId);
        }

        Texture(const char* filename):
            m_texId(0),
            width(0),
            height(0),
            channels(0),
            data(nullptr)
        {
            glGenTextures(1, &m_texId);
            loadImage(filename);
        }

        Texture& bind() {
            glBindTexture(GL_TEXTURE_2D, m_texId);
            return *this;
        };

        Texture& setWrappingX(Texture::Wrap wrapping) {
            glTextureParameteri(m_texId, GL_TEXTURE_WRAP_S, (GLint) wrapping);
            return *this;
        };
        Texture& setWrappingY(Texture::Wrap wrapping) {
            glTextureParameteri(m_texId, GL_TEXTURE_WRAP_T, (GLint) wrapping);
            return *this;
        };
        Texture& setWrapping(Texture::Wrap wrapping) {
            setWrappingX(wrapping);
            setWrappingY(wrapping);
            return *this;
        };
        Texture& setWrapping(Texture::Wrap wrappingX, Texture::Wrap wrappingY) {
            setWrappingX(wrappingX);
            setWrappingY(wrappingY);
            return *this;
        };

        Texture& setMinFilter(Texture::MinFilter f) {
            glTextureParameteri(m_texId, GL_TEXTURE_MIN_FILTER, (GLint) f);
            return *this;
        };
        Texture& setMagFilter(Texture::MagFilter f) {
            glTextureParameteri(m_texId, GL_TEXTURE_MAG_FILTER, (GLint) f);
            return *this;
        };

        Texture& loadImage(const char* filename);
        Texture& upload();

        ~Texture() {
            glDeleteTextures(1, &m_texId);
            stbi_image_free(data);
        }

    private:
        GLuint m_texId;
        int width, height, channels;
        unsigned char* data;
    };
}
