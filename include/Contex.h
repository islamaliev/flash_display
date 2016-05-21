#pragma once

namespace flash {

namespace display {
    class DisplayObject;
    class Texture;
}

namespace math {
    class Mat4;
}

namespace render {
    class Context {
    public:
        void init(unsigned width, unsigned height);

        void start(display::DisplayObject& displayObject);

        void stop();

        void dispose();

        void setMatrix(const flash::math::Mat4& matrix);

        void setProjection(const flash::math::Mat4& matrix);

        void setTexture(display::Texture*);

        void unsetTexture();
    };

}

}