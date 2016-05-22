#pragma once


#include "DisplayObject.h"

namespace flash {

namespace display {

    class Texture;

    class Image : public DisplayObject {
    public:
        void setTexture(Texture* texture);

        void draw(flash::render::Context&, flash::render::RenderState&) override;

        void setWidth(float value) override;

        void setHeight(float value) override;

    private:
        // TODO: should be const?
        Texture* m_texture;
    };

}

}
