#pragma once


namespace flash {

namespace core {

    class Color {
    public:
        static constexpr unsigned char BYTE_MASK = 0xFF;
        static constexpr float BYTE_MASK_F = BYTE_MASK;
        static constexpr float FLOAT_CONVERTER = 1.f / BYTE_MASK;

        Color() = default;

        Color(unsigned char r, unsigned char g, unsigned char b)
			: m_r(r)
			, m_g(g)
			, m_b(b) 
		{}

        explicit Color(unsigned value) 
		{
            set(value);
        }

		unsigned char r() const
		{
			return m_r;
		}

		unsigned char g() const
		{
			return m_g;
		}

		unsigned char b() const
		{
			return m_b;
		}

        float R() const 
		{
            return static_cast<float>(m_r * FLOAT_CONVERTER);
        }

        float G() const 
		{
            return static_cast<float>(m_g * FLOAT_CONVERTER);
        }

        float B() const 
		{
            return static_cast<float>(m_b * FLOAT_CONVERTER);
        }

        void setR(float value) 
		{
			m_r = static_cast<unsigned char>(value * BYTE_MASK_F);
        }

        void setG(float value) 
		{
            m_g = static_cast<unsigned char>(value * BYTE_MASK_F);
        }

        void setB(float value) 
		{
            m_b = static_cast<unsigned char>(value * BYTE_MASK_F);
        }

        void set(unsigned char r, unsigned char g, unsigned char b) 
		{
            this->m_r = r;
            this->m_g = g;
            this->m_b = b;
        }

        void set(float r, float g, float b) 
		{
			setR(r);
			setG(g);
			setB(b);
        }

        void set(unsigned value) 
		{
            m_b = value & BYTE_MASK;
            m_g = value >> 8 & BYTE_MASK;
            m_r = value >> 16 & BYTE_MASK;
        }

        Color operator+(const Color& other) const 
		{
            Color result = *this;
            result += other;
            return result;
        }

        Color& operator+=(const Color& other) 
		{
            m_r += other.m_r;
            m_g += other.m_g;
            m_b += other.m_b;
            return *this;
        }

        Color operator-(const Color& other) const 
		{
            Color result = *this;
            result -= other;
            return result;
        }

        Color& operator-=(const Color& other) 
		{
            m_r -= other.m_r;
            m_g -= other.m_g;
            m_b -= other.m_b;
            return *this;
        }

        Color operator*(const Color& other) const 
		{
            Color result = *this;
            result *= other;
            return result;
        }

        Color& operator*=(const Color& other) 
		{
            m_r *= other.m_r;
            m_g *= other.m_g;
            m_b *= other.m_b;
            return *this;
        }

        Color operator/(const Color& other) const 
		{
            Color result = *this;
            result /= other;
            return result;
        }

        Color& operator/=(const Color& other) 
		{
            m_r /= other.m_r;
            m_g /= other.m_g;
            m_b /= other.m_b;
            return *this;
        }

        Color operator+(unsigned char scalar) const 
		{
            return Color(m_r + scalar, m_g + scalar, m_b + scalar);
        }

        Color& operator+=(unsigned char scalar) 
		{
            m_r += scalar;
            m_g += scalar;
            m_b += scalar;
            return *this;
        }

        Color operator-(unsigned char scalar) const 
		{
            return Color(m_r - scalar, m_g - scalar, m_b - scalar);
        }

        Color& operator-=(unsigned char scalar) 
		{
            m_r -= scalar;
            m_g -= scalar;
            m_b -= scalar;
            return *this;
        }

		Color operator*(float scalar) const
		{
			return Color(m_r * scalar, m_g * scalar, m_b * scalar);
		}

		Color& operator*=(float scalar)
		{
			m_r *= scalar;
			m_g *= scalar;
			m_b *= scalar;
			return *this;
		}

		Color operator/(float scalar) const
		{
			return Color(m_r / scalar, m_g / scalar, m_b / scalar);
		}

		Color& operator/=(float scalar)
		{
			m_r /= scalar;
			m_g /= scalar;
			m_b /= scalar;
			return *this;
		}

        unsigned uint() const 
		{
            unsigned result(m_b);
            result |= m_g << 8;
            result |= m_r << 16;
            return result;
        }
		
	private:
        unsigned char m_r{0};
        unsigned char m_g{0};
        unsigned char m_b{0};
    };

}

}

