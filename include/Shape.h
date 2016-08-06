#pragma once

#include "DisplayObject.h"

namespace flash {

namespace display {

    class Shape : public DisplayObject {
    public:
        void preRender(flash::render::RenderState& renderState) override;
    };

}

}