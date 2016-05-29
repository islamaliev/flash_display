#include <assert.h>
#include "SpatialComponent.h"

using namespace flash;

const Entity& SpatialComponentContainer::createIndex() {
    assert(nextIndex < m_comps.size());
    unsigned entityIndex = nextIndex;
    if (m_freeIndexes.size()) {
        entityIndex = m_freeIndexes.back();
        m_freeIndexes.pop_back();
    }
    Entity& entity = m_entities[entityIndex];
    new (&m_comps[nextIndex]) SpatialComponent();
    m_dataIndexes[entityIndex] = nextIndex++;
    entity = entityIndex;
    return entity;
}

void SpatialComponentContainer::removeIndex(const Entity& index) {
    using std::swap;
    --nextIndex;
    m_freeIndexes.push_back(index);
    unsigned swapIndex = (unsigned) (std::find(m_dataIndexes.cbegin(), m_dataIndexes.cend(), nextIndex) - m_dataIndexes.cbegin());
    m_dataIndexes[swapIndex] = m_dataIndexes[index];
    swap(m_comps[m_dataIndexes[swapIndex]], m_comps[nextIndex]);
}

SpatialComponent& SpatialComponentContainer::getComponent(Entity index) {
    return m_comps[m_dataIndexes[index]];
}

void SpatialComponentContainer::forEachComponent(std::function<void(SpatialComponent&)> f) {
    std::for_each(m_comps.begin() + 1, m_comps.begin() + nextIndex, f);
}
