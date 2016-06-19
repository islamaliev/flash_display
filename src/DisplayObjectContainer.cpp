#include <RenderState.h>
#include "DisplayObjectContainer.h"
#include "Contex.h"

using namespace flash::display;

using Context = flash::render::Context;
using RenderState = flash::render::RenderState;
using Rectangle = flash::core::Rectangle;

DisplayObjectContainer::DisplayObjectContainer()
    : DisplayObject() {
    setActualWidth(1);
    setActualHeight(1);
    _setWidth(1);
    _setHeight(1);
}

void DisplayObjectContainer::draw(Context& context, RenderState& renderState) {
    DisplayObject::_updateOrderIndex(renderState.orderIndex);
    for (auto child : m_children) {
        ++renderState.orderIndex;
        child->draw(context, renderState);
    }
}

Rectangle DisplayObjectContainer::getBounds(DisplayObject* targetSpace) const {
    return Rectangle();
}