#include <RenderState.h>
#include "DisplayObjectContainer.h"
#include "Contex.h"

using namespace flash::display;

using Context = flash::render::Context;
using RenderState = flash::render::RenderState;
using Rectangle = flash::core::Rectangle;

void DisplayObjectContainer::draw(Context& context, RenderState& renderState) {
    flash::math::Mat4 backup = renderState.transform;
    renderState.transform *= getTransform();
    for (auto child : m_children) {
        child->draw(context, renderState);
    }
    renderState.transform = backup;
}

Rectangle DisplayObjectContainer::getBounds(DisplayObject* targetSpace) const {
    return Rectangle();
}