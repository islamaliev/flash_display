#pragma once


namespace flash {

namespace core {

    class Color {
    public:
        static constexpr unsigned char BYTE_MASK = 0xFF;
        static constexpr float FLOAT_CONVERTER = 1.f / BYTE_MASK;

        Color() = default;

        Color(unsigned char r, unsigned char g, unsigned char b)
                : r(r)
                , g(g)
                , b(b) {}

        explicit Color(unsigned value) {
            set(value);
        }

        float R() const {
            return (float) (r * FLOAT_CONVERTER);
        }

        float G() const {
            return (float) (g * FLOAT_CONVERTER);
        }

        float B() const {
            return (float) (b * FLOAT_CONVERTER);
        }

        void setR(float value) {
            r = value * BYTE_MASK;
        }

        void setG(float value) {
            g = value * BYTE_MASK;
        }

        void setB(float value) {
            b = value * BYTE_MASK;
        }

        void set(unsigned char r, unsigned char g, unsigned char b) {
            this->r = r;
            this->g = g;
            this->b = b;
        }

        void set(float R, float G, float B) {
            r = R * BYTE_MASK;
            g = G * BYTE_MASK;
            b = B * BYTE_MASK;
        }

        void set(unsigned value) {
            b = value & BYTE_MASK;
            g = value >> 8 & BYTE_MASK;
            r = value >> 16 & BYTE_MASK;
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

        Color operator+(unsigned char scalar) const {
            return Color(r + scalar, g + scalar, b + scalar);
        }

        Color& operator+=(unsigned char scalar) {
            r += scalar;
            g += scalar;
            b += scalar;
            return *this;
        }

        Color operator-(unsigned char scalar) const {
            return Color(r - scalar, g - scalar, b - scalar);
        }

        Color& operator-=(unsigned char scalar) {
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
            return *this * (1.f / scalar);
        }

        Color& operator/=(float scalar) {
            *this *= 1.f / scalar;
            return *this;
        }

        unsigned uint() const {
            unsigned result(b);
            result |= g << 8;
            result |= r << 16;
            return result;
        }

        unsigned char r{0};
        unsigned char g{0};
        unsigned char b{0};
    };

}

}

