#pragma once

namespace flash {

namespace core {
    class Color;
}

using Color = flash::core::Color;

namespace display {

    class Texture {
    public:
        // TODO: move it to some kind of texture manager
		static int s_numTextures;

        Texture(int width, int height);

        Texture(const Texture& other) = delete;
        Texture& operator=(const Texture& other) = delete;

        Texture(Texture&& other) noexcept
            : m_id(other.m_id)
            , m_width(other.m_width)
            , m_height(other.m_height)
            , m_data(other.m_data) {
            other.m_id = 0;
            other.m_width = 0;
            other.m_height = 0;
            other.m_data = nullptr;
        }

        Texture& operator=(Texture&& other) noexcept;

        ~Texture();

        void dispose();

        int width() const { return m_width; }
        int height() const { return m_height; }

        const Color* data() const { return m_data; }
        Color* data() { return m_data; }

        unsigned getId() const { return m_id; }

        void bindData();

    private:
        unsigned m_id{0};
        int m_width{0};
        int m_height{0};
        Color* m_data{nullptr};

    };

}

}
