#pragma once

#include "DisplayObjectContainer.h"

namespace flash {

namespace display {

    class Stage : public DisplayObjectContainer {
    public:
        Stage(unsigned width, unsigned height) {
            setWidth(width);
            setHeight(height);
        }

        void start();

        void stop();

        void draw(flash::render::Context&, flash::render::RenderState&) override;
    };

}

}
