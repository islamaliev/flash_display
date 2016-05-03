#include "DisplayObjectContainer.h"
#include "Contex.h"

using namespace flash::display;

using Context = flash::render::Context;
using Rectangle = flash::core::Rectangle;

void DisplayObjectContainer::draw(Context& context) {
    for (auto child : m_children) {
        child->draw(context);
    }
}

Rectangle DisplayObjectContainer::getBounds(DisplayObject* targetSpace) const {
    return Rectangle();
}