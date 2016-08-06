#include "Shape.h"
#include "RenderState.h"
#include "Contex.h"

using namespace flash::display;

void Shape::preRender(flash::render::RenderState& renderState) {
    if (m_visible) {
        s_context->unsetTexture();
    }
    DisplayObject::preRender(renderState);
}

