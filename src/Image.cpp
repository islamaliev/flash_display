#include "Image.h"
#include "Contex.h"
#include "Texture.h"

using namespace flash::display;

void Image::draw(flash::render::Context& context, flash::render::RenderState& renderState) {
    if (m_texture) {
        context.setTexture(m_texture);
    }
    DisplayObject::draw(context, renderState);
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