#pragma once

namespace flash {

namespace display {
    class DisplayObject;
}

namespace math {
    class Mat4;
}

namespace render {
    class Context {
    public:
        void init(unsigned width, unsigned height);

        void start(display::DisplayObject& displayObject);

        void dispose();

        void setMatrix(const flash::math::Mat4& matrix);
    };

}

}