#pragma once
#include "core/AirCore.h"

typedef uint16_t pixel_t;

struct Pixel {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

class GridTexture {
public:
    GridTexture(pixel_t size_x = 32, pixel_t size_y = 32){
        glGenTextures(1, &m_tex_id);
        resize(size_x, size_y);
    }

    void resize(pixel_t size_x, pixel_t size_y){
        m_size_x = size_x;
        m_size_y = size_y;
        m_pixel_data.resize(size_x * size_y);
        update();
    }

    void set_pixel(pixel_t x, pixel_t y, Pixel const& pixel){
        m_pixel_data[y * m_size_x + x] = pixel;
    }

    Pixel get_pixel(pixel_t x, pixel_t y){
        return m_pixel_data[y * m_size_x + x];
    }

    void update(){
        glBindTexture(GL_TEXTURE_2D, m_tex_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_size_x, m_size_y, 0, GL_RGB, GL_UNSIGNED_BYTE, m_pixel_data.data());
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void bind_texture(){
        glBindTexture(GL_TEXTURE_2D, m_tex_id);
    }

    void unbind_texture(){
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glm::vec2 size(){
        return glm::vec2(m_size_x, m_size_y);
    }
private:
    pixel_t m_size_x, m_size_y;
    std::vector<Pixel> m_pixel_data;

    // OGL
    GLuint m_tex_id;
};