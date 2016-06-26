#include <Image.h>
#include "DisplayObject.h"
#include "DisplayObjectContainer.h"
#include "Contex.h"
#include "RenderState.h"

using namespace flash::display;
using namespace flash::render;

using Mat4 = flash::math::Mat4;

// TODO: make it configurable
flash::ComponentContainer* DisplayObject::s_components = new flash::ComponentContainer(160000);

DisplayObject::DisplayObject() {
    m_index = s_components->createEntity();
}

DisplayObject::~DisplayObject() {
    // TODO: set all nested method to noexcept
    if (m_parent)
        m_parent->removeChild(this);
    s_components->removeEntity(m_index);
}

void DisplayObject::draw(Context& context, RenderState& renderState) {
    DisplayObject::_updateOrderIndex(renderState.orderIndex);

    // TODO: ugly stuff
    if (!dynamic_cast<Image*>(this)) {
        context.unsetTexture();
    }
}

Mat4 DisplayObject::getTransform() const {
    Mat4 m;
    float xt = x() - pivotX() * scaleX();
    float yt = y() - pivotY() * scaleY();
    m.translate(xt, yt, 0);
    m.scale(width(), height(), 0);
    return m;
}

Mat4 DisplayObject::getTransform(DisplayObjectContainer* targetSpace) const {
    const DisplayObjectContainer* currentParent = getParent();
    if (!currentParent) {
        return Mat4::IDENTITY;
    }
    if (targetSpace == currentParent) {
        return getTransform();
    }
    Mat4 m = getTransform();

    while (currentParent && currentParent != targetSpace) {
        const Mat4& mat4 = currentParent->getTransform();
        m = mat4 * m;
        currentParent = currentParent->getParent();
    }
    return m;
}

void DisplayObject::setWidth(float value) {
    setActualWidth(value);
}

void DisplayObject::setHeight(float value) {
    setActualHeight(value);
}

void DisplayObject::setScaleX(float value) {
    _setScaleX(value);
    _setWidth(_getActualWidth() * value);
}

void DisplayObject::setScaleY(float value) {
    _setScaleY(value);
    _setHeight(_getActualHeight() * value);
}

const flash::SpatialComponent& DisplayObject::_spatial() const {
    return s_components->getSpatialComponent(m_index);
}

int DisplayObject::depth() const {
    return s_components->getDepthComponent(m_index);
}

int DisplayObject::orderIndex() const {
    return s_components->getOrderComponent(m_index);
}

void DisplayObject::_setDepth(int value) {
    s_components->getDepthComponent(m_index) = value;
}

void DisplayObject::_alterTreeSizeBy(int value) {
    _setTreeSize(treeSize() + value);
    if (m_parent)
        m_parent->_alterTreeSizeBy(value);
}

void DisplayObject::_updateOrderIndex(int& orderIndex) {
    s_components->getOrderComponent(m_index) = orderIndex;
}

flash::ComponentContainer& DisplayObject::_getComponents() {
    return *s_components;
}