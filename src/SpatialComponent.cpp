#include <assert.h>
#include "SpatialComponent.h"

using namespace flash;

unsigned SpatialComponentContainer::createIndex() {
    assert(nextIndex < comps.size());
    return nextIndex++;
}

void SpatialComponentContainer::removeIndex(unsigned index) {
    using std::swap;
    --nextIndex;
    swap(comps[index], comps[nextIndex]);
}

SpatialComponent& SpatialComponentContainer::getComponent(unsigned index) {
    return comps[index];
}
