#pragma once

namespace flash {

namespace render {

    class Context {
    public:
        void init(unsigned width, unsigned height);

        void start();

        void dispose();
    };

}

}