#include "Image.h"
#include "Contex.h"
#include "Texture.h"

using namespace flash::display;

void Image::preRender(flash::render::RenderState& renderState) {
    if (m_texture && visible()) {
        s_context->setTexture(m_texture);
    }
    DisplayObject::preRender(renderState);
}

void Image::setTexture(Texture* texture) {
    m_texture = texture;
    setActualWidth(texture->width());
    setActualHeight(texture->height());
}

void Image::setWidth(float value) {
    _setWidth(value);
    _setScaleX(value / _getActualWidth());
}

void Image::setHeight(float value) {
    _setHeight(value);
    _setScaleY(value / _getActualHeight());
}