#include "Stage.h"
#include "Contex.h"

using namespace flash;
using namespace display;

using Context = flash::render::Context;

void Stage::start() {
    Context context;
    context.init((unsigned) width(), (unsigned) height());
    context.start(*this);
    context.dispose();
}

void Stage::draw(Context& context) {
    DisplayObjectContainer::draw(context);
}