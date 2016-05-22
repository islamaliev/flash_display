#include "Image.h"
#include "Contex.h"

using namespace flash::display;

void Image::draw(flash::render::Context& context, flash::render::RenderState& renderState) {
    if (m_texture) {
        context.setTexture(m_texture);
    }
    DisplayObject::draw(context, renderState);
}
