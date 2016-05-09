#pragma once


#include "DisplayObject.h"

namespace flash {

namespace display {

    class Texture;

    class Bitmap : public DisplayObject {
    public:
        void setTexture(const Texture* texture) {
            m_texture = texture;
        }

        void draw(flash::render::Context&, flash::render::RenderState&);

    private:
        const Texture* m_texture;

    };

}

}
