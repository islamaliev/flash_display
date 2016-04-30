#pragma once

namespace flash {

namespace display {
    class DisplayObject;
}

namespace render {
    class Context {
    public:
        void init(unsigned width, unsigned height);

        void start(display::DisplayObject& displayObject);

        void dispose();
    };

}

}