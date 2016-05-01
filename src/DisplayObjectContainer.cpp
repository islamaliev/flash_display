#include "DisplayObjectContainer.h"
#include "Contex.h"

using namespace flash::display;

using Context = flash::render::Context;

void DisplayObjectContainer::draw(Context& context) {
    for (auto child : m_children) {
        child->draw(context);
    }
}
