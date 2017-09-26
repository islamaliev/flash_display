#include "Image.h"
#include "Context.h"
#include "Texture.h"

using namespace flash::display;

void Image::preRender(flash::render::RenderState& renderState) {
//    if (m_texture && m_visible) {
//        s_context->setTexture(m_texture);
//    }
    DisplayObject::preRender(renderState);
}

void Image::setTexture(Texture* texture) {
    m_texture = texture;
    _setTextureId(m_texture->getId());
    setActualWidth(static_cast<float>(texture->width()));
    setActualHeight(static_cast<float>(texture->height()));
}

void Image::setWidth(float value) {
    _setWidth(value);
    _setScaleX(value / _getActualWidth());
}

void Image::setHeight(float value) {
    _setHeight(value);
    _setScaleY(value / _getActualHeight());
}