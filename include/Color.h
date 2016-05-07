#pragma once


namespace flash {

namespace core {

    class Color {
    public:
        static constexpr unsigned char BYTE_MASK = 0xFF;
        static constexpr float FLOAT_CONVERTER = 1.f / BYTE_MASK;

        Color() = default;

        Color(float r, float g, float b)
                : r(r)
                , g(g)
                , b(b) {}

        explicit Color(unsigned value) {
            set(value);
        }

        unsigned char R() const {
            return (unsigned char) (r * BYTE_MASK);
        }

        unsigned char G() const {
            return (unsigned char) (g * BYTE_MASK);
        }

        unsigned char B() const {
            return (unsigned char) (b * BYTE_MASK);
        }

        void setR(unsigned char value) {
            r = value * FLOAT_CONVERTER;
        }

        void setG(unsigned char value) {
            g = value * FLOAT_CONVERTER;
        }

        void setB(unsigned char value) {
            b = value * FLOAT_CONVERTER;
        }

        void set(float r, float g, float b) {
            this->r = r;
            this->g = g;
            this->b = b;
        }

        void set(unsigned char R, unsigned char G, unsigned char B) {
            r = R * FLOAT_CONVERTER;
            g = G * FLOAT_CONVERTER;
            b = B * FLOAT_CONVERTER;
        }

        void set(unsigned value) {
            b = (value & BYTE_MASK) * FLOAT_CONVERTER;
            g = (value >> 8 & BYTE_MASK) * FLOAT_CONVERTER;
            r = (value >> 16 & BYTE_MASK) * FLOAT_CONVERTER;
        }

        Color operator+(const Color& other) const {
            Color result = *this;
            result += other;
            return result;
        }

        Color& operator+=(const Color& other) {
            r += other.r;
            g += other.g;
            b += other.b;
            return *this;
        }

        Color operator-(const Color& other) const {
            Color result = *this;
            result -= other;
            return result;
        }

        Color& operator-=(const Color& other) {
            r -= other.r;
            g -= other.g;
            b -= other.b;
            return *this;
        }

        Color operator*(const Color& other) const {
            Color result = *this;
            result *= other;
            return result;
        }

        Color& operator*=(const Color& other) {
            r *= other.r;
            g *= other.g;
            b *= other.b;
            return *this;
        }

        Color operator/(const Color& other) const {
            Color result = *this;
            result /= other;
            return result;
        }

        Color& operator/=(const Color& other) {
            r /= other.r;
            g /= other.g;
            b /= other.b;
            return *this;
        }

        Color operator+(float scalar) const {
            return Color(r + scalar, g + scalar, b + scalar);
        }

        Color& operator+=(float scalar) {
            r += scalar;
            g += scalar;
            b += scalar;
            return *this;
        }

        Color operator-(float scalar) const {
            return Color(r - scalar, g - scalar, b - scalar);
        }

        Color& operator-=(float scalar) {
            r -= scalar;
            g -= scalar;
            b -= scalar;
            return *this;
        }

        Color operator*(float scalar) const {
            return Color(r * scalar, g * scalar, b * scalar);
        }

        Color& operator*=(float scalar) {
            r *= scalar;
            g *= scalar;
            b *= scalar;
            return *this;
        }

        Color operator/(float scalar) const {
            float div = 1 / scalar;
            return *this * div;
        }

        Color& operator/=(float scalar) {
            float div = 1 / scalar;
            *this *= div;
            return *this;
        }

        unsigned uint() const {
            unsigned char rr = (unsigned char) (r * BYTE_MASK);
            unsigned char gg = (unsigned char) (g * BYTE_MASK);
            unsigned char bb = (unsigned char) (b * BYTE_MASK);
            unsigned result(bb);
            result |= rr << 16;
            result |= gg << 8;
            return result;
        }

        void clamp() {
            if (r > 1) r = 1;
            if (g > 1) g = 1;
            if (b > 1) b = 1;
        }

        float r{0};
        float g{0};
        float b{0};
    };

}

}

