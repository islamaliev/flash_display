#include <Mat4.h>
#include <RenderState.h>
#include "Stage.h"
#include "Context.h"

using namespace flash;
using namespace display;

using Context = flash::render::Context;
using RenderState = flash::render::RenderState;
using Mat4 = flash::math::Mat4;

namespace {
    bool _isInitialized{false};
    bool _isStarted{false};
}

Stage::Stage(unsigned width, unsigned height) {
	_setWidth(static_cast<float>(width));
	_setHeight(static_cast<float>(height));
	_setDepth(0);
	init();
}

void Stage::start() {
    if (!_isStarted) {
        _isStarted = true;
        init();
        s_context->start(*this);
    }
}

void Stage::stop() {
    if (_isStarted) {
        _isStarted = _isInitialized = false;
        s_context->stop();
    }
}

void Stage::init() {
    if (!_isInitialized) {
        _isInitialized = true;
        s_context->init((unsigned) width(), (unsigned) height());
        // TODO: max depth (far) should be configurable of calculated in more sophisticated way
        s_context->setProjection(Mat4::orthographicProjection(0, width(), 0, height(), -1, 1000));
    }
}

void Stage::clear() {
    _alterTreeSizeBy(1 - treeSize());
    for (auto child : m_children) {
        child->_setParent(nullptr);
        child->destroy();
    }
    m_children.clear();
}
