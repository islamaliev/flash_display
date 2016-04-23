#pragma once

#include <ostream>


namespace flash {

namespace core {

    class Rectangle {
    public:
        Rectangle(float x, float y, float w, float h)
            : x(x)
            , y(y)
            , width(w)
            , height(h)
        {}

        Rectangle() = default;

        bool equals(const Rectangle& otherRect) const {
            return *this == otherRect;
        }

        bool operator==(const Rectangle& otherRect) const {
            return !(x != otherRect.x || y != otherRect.y || width != otherRect.width || height != otherRect.height);
        }

        bool operator!=(const Rectangle& otherRect) const {
            return !(*this == otherRect);
        }

        friend std::ostream& operator<<(std::ostream& os, const Rectangle& rect) {
            return os << "(x = " << rect.x << ", y = " << rect.y << ", width = " << rect.width << ", height = " << rect.height << ") ";
        }

        float x{0};
        float y{0};
        float width{1};
        float height{1};
    };

}

}
