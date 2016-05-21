#pragma once


#include "DisplayObject.h"

namespace flash {

namespace display {

    class Texture;

    class Bitmap : public DisplayObject {
    public:
        void setTexture(Texture* texture) {
            m_texture = texture;
        }

        void draw(flash::render::Context&, flash::render::RenderState&);

    private:
        // TODO: should be const?
        Texture* m_texture;
    };

}

}
