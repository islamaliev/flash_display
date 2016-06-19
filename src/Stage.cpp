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
}

Stage::Stage(unsigned width, unsigned height) {
    _setWidth(width);
    _setHeight(height);
    _setDepth(0);
}

void Stage::start() {
    context.init((unsigned) width(), (unsigned) height());
    context.setProjection(Mat4::orthographicProjection(0, width(), 0, height(), -1, 20));
    context.start(*this);
}

void Stage::stop() {
    context.stop();
}
