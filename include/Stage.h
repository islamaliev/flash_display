#pragma once

#include "DisplayObjectContainer.h"

namespace flash {

namespace display {

    class Stage : public DisplayObjectContainer {
    public:
        Stage(unsigned width, unsigned height);

        void start();

        void stop();

        void draw(flash::render::Context&, flash::render::RenderState&) override;

        void updateOrder() {
            int i = 0;
            _updateOrderIndex(i);
        }
    };

}

}
