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
    m_width = value;
    m_scaleX = value / m_actualWidth;
}

void Image::setHeight(float value) {
    m_height = value;
    m_scaleY = value / m_actualHeight;
}