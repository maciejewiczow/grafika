#include "Texture.h"

namespace gl
{
    Texture& Texture::loadImage(const char* filename) {
        stbi_set_flip_vertically_on_load(true);
        data = stbi_load(filename, &width, &height, &channels, 0);

        if (!data)
            throw image_load_exception{ stbi_failure_reason() };

        return *this;
    }

    Texture& Texture::upload() {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        return *this;
    }
}
