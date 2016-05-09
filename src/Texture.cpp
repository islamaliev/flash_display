#include "Texture.h"
#include "Color.h"

using namespace flash::display;

using Color = flash::core::Color;

Texture::Texture(unsigned width, unsigned height)
        : m_width(width)
        , m_height(height) {
    m_data = new Color[width * height];
}

Texture::~Texture() {
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