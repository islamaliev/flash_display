#pragma once

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

        float x{0};
        float y{0};
        float width{1};
        float height{1};
    };

}

}
