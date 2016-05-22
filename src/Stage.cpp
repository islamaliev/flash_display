#include <Mat4.h>
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

void Stage::start() {
    context.init((unsigned) width(), (unsigned) height());
    context.setProjection(Mat4::orthographicProjection(0, width(), 0, height(), -1, 20));
    context.start(*this);
}

void Stage::stop() {
    context.stop();
}

void Stage::draw(Context& context, RenderState& renderState) {
    for (auto child : m_children) {
        child->draw(context, renderState);
    }
}