#pragma once
#include "../GL.hpp"
#include <filesystem>
#include <string>

namespace Modeye::Gfx
{

class Texture
{
public:
    Texture();
    explicit Texture(const std::filesystem::path& filepath);
    Texture(int width, int height, const uint8_t* rgbaData);
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    void bind(GLuint slot = 0) const;
    static void unbind(GLuint slot = 0);

    GLuint id() const { return m_id; }
    int width() const { return m_width; }
    int height() const { return m_height; }
    bool isValid() const { return m_id != 0; }

    static Texture createDefaultWhite();
    static Texture createCheckerboard(int size = 64, int tileSize = 8);

private:
    GLuint m_id = 0;
    int m_width = 0;
    int m_height = 0;
    int m_channels = 0;
};

}
