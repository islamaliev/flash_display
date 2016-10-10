#pragma once

#include "DisplayObject.h"

namespace flash {

namespace display {

    class Shape : public DisplayObject {
    public:
        Shape(){
            // for shapes the index is -1 so that it can be checked in ths frag shader
            // all other indices must be from 0 to GL_MAX_TEXTURE_IMAGE_UNITS
            _setTextureId(-1);
        }
        
        void preRender(flash::render::RenderState& renderState) override {
            DisplayObject::preRender(renderState);
        };
    };

}

}