#pragma once

namespace flash {

namespace display {
    class DisplayObject;
    class Texture;
}

namespace math {
    class Mat4;
}

namespace allocator {
    class StackAllocator;
}

class ComponentContainer;

namespace render {
    struct BufferData {
        void* matrices;
        void* textures;
        unsigned matricesSize;
        unsigned texturesSize;
    };

    class Context {
    public:
        class TransformationsBufferOrganizer {
        public:
            static void organize(display::DisplayObject& stage, allocator::StackAllocator&, BufferData&);
        };

        void init(unsigned width, unsigned height);

        void start(display::DisplayObject& displayObject);

        void stop();

        void dispose();

        void setProjection(const flash::math::Mat4& matrix);

        void setTexture(display::Texture*);

        void unsetTexture();
    };
}

}