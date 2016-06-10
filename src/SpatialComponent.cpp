#include <assert.h>
#include "SpatialComponent.h"

using namespace flash;

/*
 *  container = SpatialComponentContainer(1000);
 *
 *  e = container.createEntity();
 *  c = container.getSpatialComponent(e);
 *  c.width = 10;
 *
 *  container.forEachComponent([](auto& c) { c.width += 1; });
 */

const Entity& SpatialComponentContainer::createEntity() {
    assert(nextIndex < m_comps.size());
    unsigned entityIndex = nextIndex;
    if (m_freeIndexes.size()) {
        entityIndex = m_freeIndexes.back();
        m_freeIndexes.pop_back();
    }
    Entity& entity = m_entities[entityIndex];
    new (&m_comps[nextIndex]) SpatialComponent();
    new (&m_depths[nextIndex]) int(-1);
    new (&m_order[nextIndex]) int(-1);
    m_dataIndexes[entityIndex] = nextIndex++;
    entity = entityIndex;
    return entity;
}

void SpatialComponentContainer::removeEntity(const Entity& e) {
    using std::swap;
    --nextIndex;
    m_freeIndexes.push_back(e);
    unsigned swapIndex = (unsigned) (std::find(m_dataIndexes.cbegin(), m_dataIndexes.cend(), nextIndex) - m_dataIndexes.cbegin());
    m_dataIndexes[swapIndex] = m_dataIndexes[e];
    swap(m_comps[m_dataIndexes[swapIndex]], m_comps[nextIndex]);
}

SpatialComponent& SpatialComponentContainer::getSpatialComponent(Entity e) {
    return m_comps[m_dataIndexes[e]];
}

int& SpatialComponentContainer::getDepthComponent(Entity e) {
    return m_depths[m_dataIndexes[e]];
}

int& SpatialComponentContainer::getOrderComponent(Entity e) {
    return m_order[m_dataIndexes[e]];
}

void SpatialComponentContainer::forEachComponent(std::function<void(SpatialComponent&)> f) {
    std::for_each(m_comps.begin() + 1, m_comps.begin() + nextIndex, f);
}
