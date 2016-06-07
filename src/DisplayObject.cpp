#include <GL/glew.h>
#include <Image.h>
#include "DisplayObject.h"
#include "DisplayObjectContainer.h"
#include "Contex.h"
#include "RenderState.h"

using namespace flash::display;
using namespace flash::render;

using Mat4 = flash::math::Mat4;

namespace {
    const unsigned FLOAT_PRO_POINT = 3;
//    const unsigned POINTS_NUM = 6;

    GLuint _vao;

    bool initialized = false;

    void initVAO(GLuint& vao) {
        float points[] = {
                0.0f,  1.0f,  0.0f,
                1.0f, 1.0f,  0.0f,
                1.0f, 0.0f,  0.0f,
                0.0f, 0.0f,  0.0f
        };

        GLint indecies[] = {
                0, 1, 2,
                3, 0, 2
        };

        GLuint vertexBuffer = 0;
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

        vao = 0;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glVertexAttribPointer(0, FLOAT_PRO_POINT, GL_FLOAT, GL_FALSE, 0, NULL);

        GLuint indicesBuffer = 0;
        glGenBuffers(1, &indicesBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indecies), indecies, GL_STATIC_DRAW);
    }

    flash::SpatialComponentContainer components = flash::SpatialComponentContainer(1000);
}

DisplayObject::DisplayObject() {
    m_index = components.createEntity();
}

void DisplayObject::draw(Context& context, RenderState& renderState) {
    if (!initialized) {
        initVAO(_vao );
        initialized = true;
    }

    // TODO: ugly stuff
    if (!dynamic_cast<Image*>(this)) {
        context.unsetTexture();
    }

    context.setMatrix(renderState.transform * getTransform());

    glBindVertexArray(_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
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
        m.multiplyByMatrix(mat4);
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
    return components.getSpatialComponent(m_index);
}

int DisplayObject::depth() const {
    return components.getDepthComponent(m_index);
}

void DisplayObject::_setDepth(int value) {
    components.getDepthComponent(m_index) = value;
}

void DisplayObject::_alterTreeSizeBy(int value) {
    _setTreeSize(treeSize() + value);
    if (m_parent)
        m_parent->_alterTreeSizeBy(value);
}