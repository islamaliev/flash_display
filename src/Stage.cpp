#include <Mat4.h>
#include <RenderState.h>
#include "Stage.h"
#include "Contex.h"

using namespace flash;
using namespace display;

using Context = flash::render::Context;
using RenderState = flash::render::RenderState;
using Mat4 = flash::math::Mat4;

namespace {
    Context context;
    bool _isInitialized{false};
    bool _isStarted{false};
}

Stage::Stage(unsigned width, unsigned height) {
    _setWidth(width);
    _setHeight(height);
    _setDepth(0);
}

void Stage::start() {
    if (!_isStarted) {
        _isStarted = true;
        init();
        context.start(*this);
    }
}

void Stage::stop() {
    if (_isStarted) {
        _isStarted = _isInitialized = false;
        context.stop();
    }
}

void Stage::init() {
    if (!_isInitialized) {
        _isInitialized = true;
        context.init((unsigned) width(), (unsigned) height());
        context.setProjection(Mat4::orthographicProjection(0, width(), 0, height(), -1, 20));
    }
}
