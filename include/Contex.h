#pragma once

namespace flash {

namespace display {
    class DisplayObject;
}

namespace math {
    class Mat4;
}

namespace render {
    struct BufferData {
        math::Mat4* matrices;
        int* textures;
        unsigned* batchSizes;
        unsigned numDraws;
    };

    class Context {
    public:
        using TextureIndexType = unsigned;
        
        void init(unsigned width, unsigned height);

        void start(display::DisplayObject& displayObject);

        void stop();

        void dispose();

        void setProjection(const math::Mat4& matrix);
    };
}

}