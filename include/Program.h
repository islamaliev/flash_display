#pragma once

namespace flash {

namespace render {

    class Context;

    class Program {
    public:
        Program();

        void init();

        void activate(Context* context);

        void dispose();
    };

}

}