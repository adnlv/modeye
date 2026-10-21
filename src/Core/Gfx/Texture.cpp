#include "Texture.hpp"
#include "../Log.hpp"
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Modeye::Gfx::Texture::Texture()
    : m_id(0), m_width(0), m_height(0), m_channels(0)
{
}

Modeye::Gfx::Texture::Texture(const std::filesystem::path& filepath)
    : m_id(0), m_width(0), m_height(0), m_channels(0)
{
    stbi_set_flip_vertically_on_load(1);

    unsigned char* data = stbi_load(filepath.string().c_str(), &m_width, &m_height, &m_channels, 4);
    if (!data) {
        Modeye::Log::error("Failed to load texture file: {}", filepath.string());
        return;
    }

    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    Modeye::Log::info("Loaded texture '{}' ({}x{}, ID: {})", filepath.string(), m_width, m_height, m_id);
}

Modeye::Gfx::Texture::Texture(int width, int height, const uint8_t* rgbaData)
    : m_id(0), m_width(width), m_height(height), m_channels(4)
{
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgbaData);
    glGenerateMipmap(GL_TEXTURE_2D);
}

Modeye::Gfx::Texture::~Texture()
{
    if (m_id != 0) {
        glDeleteTextures(1, &m_id);
        m_id = 0;
    }
}

Modeye::Gfx::Texture::Texture(Texture&& other) noexcept
    : m_id(other.m_id), m_width(other.m_width), m_height(other.m_height), m_channels(other.m_channels)
{
    other.m_id = 0;
    other.m_width = 0;
    other.m_height = 0;
    other.m_channels = 0;
}

Modeye::Gfx::Texture& Modeye::Gfx::Texture::operator=(Texture&& other) noexcept
{
    if (this != &other) {
        if (m_id != 0) {
            glDeleteTextures(1, &m_id);
        }

        m_id = other.m_id;
        m_width = other.m_width;
        m_height = other.m_height;
        m_channels = other.m_channels;

        other.m_id = 0;
        other.m_width = 0;
        other.m_height = 0;
        other.m_channels = 0;
    }
    return *this;
}

void Modeye::Gfx::Texture::bind(GLuint slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

void Modeye::Gfx::Texture::unbind(GLuint slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Modeye::Gfx::Texture Modeye::Gfx::Texture::createDefaultWhite()
{
    const uint8_t whitePixel[4] = { 255, 255, 255, 255 };
    return Texture(1, 1, whitePixel);
}

Modeye::Gfx::Texture Modeye::Gfx::Texture::createCheckerboard(int size, int tileSize)
{
    std::vector<uint8_t> pixels(size * size * 4);
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            bool isWhite = ((x / tileSize) + (y / tileSize)) % 2 == 0;
            uint8_t color = isWhite ? 240 : 60;
            size_t idx = (y * size + x) * 4;
            pixels[idx + 0] = color;
            pixels[idx + 1] = color;
            pixels[idx + 2] = color;
            pixels[idx + 3] = 255;
        }
    }
    return Texture(size, size, pixels.data());
}
