#pragma once

namespace flash {

namespace core {
    class Color;
}

using Color = flash::core::Color;

namespace display {

    class Texture {
    public:

        Texture(unsigned width, unsigned height);

        Texture(const Texture& other) = delete;
        Texture& operator=(const Texture& other) = delete;

        Texture(Texture&& other) noexcept
            : m_width(other.m_width)
            , m_height(other.m_height)
            , m_data(other.m_data) {
            other.m_data = nullptr;
        }

        Texture& operator=(Texture&& other) noexcept;

        ~Texture();

        unsigned width() const { return m_width; }
        unsigned height() const { return m_height; }

        const Color* data() const { return m_data; }
        Color* data() { return m_data; }

    private:
        unsigned m_width;
        unsigned m_height;
        Color* m_data;

    };

}

}
