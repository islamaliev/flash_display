#pragma once


#include "Mat4.h"

namespace flash {

namespace render {

    class RenderState {
    public:
        flash::math::Mat4 transform;
    };

}

}