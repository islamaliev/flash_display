#import <OpenGL/OpenGL.h>
#import <OpenGL/gl3.h>
#include "Texture.h"
#include "Color.h"

using namespace flash::display;

using Color = flash::core::Color;

unsigned Texture::s_numTextures = 0;

Texture::Texture(unsigned width, unsigned height)
        : m_width(width)
        , m_height(height) {
    m_data = new Color[width * height];
    ++s_numTextures;
}

Texture::~Texture() {
    --s_numTextures;
    if (m_id) {
        glDeleteTextures(1, &m_id);
    }
    delete [](m_data);
}

Texture& Texture::operator=(Texture&& other) noexcept {
    delete [](m_data);
    m_data = other.m_data;
    m_width = other.m_width;
    m_height = other.m_height;
    other.m_data = nullptr;
    return *this;
}

void Texture::bindData() {
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);
//    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, m_width, m_height);
//    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGB, GL_UNSIGNED_BYTE, m_data);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void Texture::dispose() {
    if (m_id) {
        glDeleteTextures(1, &m_id);
    }
}