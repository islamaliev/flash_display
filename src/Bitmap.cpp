#include "Bitmap.h"
#include "Contex.h"

using namespace flash::display;

void Bitmap::draw(flash::render::Context& context, flash::render::RenderState& renderState) {
    if (m_texture) {
        context.setTexture(m_texture);
    }
    DisplayObject::draw(context, renderState);
}
