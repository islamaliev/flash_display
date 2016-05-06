#include <Mat4.h>
#include "Stage.h"
#include "Contex.h"

using namespace flash;
using namespace display;

using Context = flash::render::Context;
using RenderState = flash::render::RenderState;
using Mat4 = flash::math::Mat4;

void Stage::start() {
    Context context;
    context.init((unsigned) width(), (unsigned) height());
    context.setProjection(Mat4::orthographicProjection(0, 800, 0, 600, -1, 20));
    context.start(*this);
    context.dispose();
}

void Stage::draw(Context& context, RenderState& renderState) {
    DisplayObjectContainer::draw(context, renderState);
}