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

void DisplayObjectContainer::addChildAt(DisplayObject* child, unsigned index) {
    DisplayObjectContainer* parent = child->getParent();
    if (parent) {
        if (parent == this) {
            _moveChildTo(child, index);
            return;
        }
        // TODO: optimize so that when child is moved depth and tree size are calculated only once
        parent->removeChild(child);
    }
    m_children.insert(m_children.begin() + index, child);
    child->_setParent(this);
    child->_updateDepth(depth());
    _alterTreeSizeBy(child->treeSize());
}

void DisplayObjectContainer::removeChildren() {
    _alterTreeSizeBy(1 - treeSize());
    for (auto child : m_children) {
        child->_resetDepth();
        child->_setParent(nullptr);
    }
    m_children.clear();
}

void DisplayObjectContainer::_resetDepth() {
    DisplayObject::_resetDepth();
    for (auto child : m_children) {
        child->_resetDepth();
    }
}

void DisplayObjectContainer::_resetOrderIndex() {
    DisplayObject::_resetOrderIndex();
    for (auto child : m_children) {
        child->_resetOrderIndex();
    }
}

void DisplayObjectContainer::_updateDepth(int parentDepth) {
    DisplayObject::_updateDepth(parentDepth);
    int d = depth();
    for (auto child : m_children) {
        child->_updateDepth(d);
    }
}

DisplayObject* DisplayObjectContainer::_removeChildAt(std::vector<DisplayObject*>::const_iterator it) {
    DisplayObject* child = *it;
    m_children.erase(it);
    child->_setParent(nullptr);
    child->_resetDepth();
    _alterTreeSizeBy(-child->treeSize());
    return child;
}

void DisplayObjectContainer::_moveChildTo(DisplayObject* child, unsigned index) {
    const auto& newIndex = m_children.begin() + index;
    if (child == *newIndex || (index > 0 && child == *(newIndex - 1))) {
        return;
    }
    const auto& currentIndex = std::find(m_children.begin(), m_children.end(), child);
    bool moveLeft = currentIndex >= newIndex;
    const auto& firstIt = moveLeft ? newIndex : currentIndex;
    const auto& lastIt = !moveLeft ? newIndex : currentIndex + 1;
    const auto& newFirstIt = moveLeft ? currentIndex : currentIndex + 1;
    std::rotate(firstIt, newFirstIt, lastIt);
}

void DisplayObjectContainer::preRender(flash::render::RenderState& renderState) {
    if (m_visible && m_children.size()) {
        _setOrderIndex(renderState.orderIndex);
        for (auto child : m_children) {
            ++renderState.orderIndex;
            child->preRender(renderState);
        }
    } else {
        --renderState.orderIndex;
        _resetOrderIndex();
    }
}

Rectangle DisplayObjectContainer::getBounds(DisplayObject* targetSpace) const {
    return Rectangle();
}

void DisplayObjectContainer::destroy() {
    for (auto child : m_children) {
        child->_setParent(nullptr);
        child->destroy();
    }
    m_children.clear();
    DisplayObject::destroy();
}