#include <assert.h>
#include "ComponentContainer.h"

using namespace flash;

/*
 *  container = ComponentContainer(1000);
 *
 *  e = container.createEntity();
 *  c = container.getSpatialComponent(e);
 *  c.width = 10;
 *
 *  container.forEachComponent([](auto& c) { c.width += 1; });
 */

const Entity& ComponentContainer::createEntity() {
    assert(nextIndex < m_comps.size());
    unsigned entityIndex = nextIndex;
    if (m_freeEntities.size()) {
        entityIndex = m_freeEntities.back();
        m_freeEntities.pop_back();
    }
    Entity& entity = m_entities[entityIndex];
    new (&m_comps[nextIndex]) SpatialComponent();
    new (&m_depths[nextIndex]) int(-1);
    new (&m_order[nextIndex]) int(-1);
    m_dataIndexes[entityIndex] = nextIndex++;
    entity = entityIndex;
    return entity;
}

void ComponentContainer::removeEntity(const Entity& e) {
    using std::swap;
    --nextIndex;
    m_freeEntities.push_back(e);
    unsigned swapIndex = (unsigned) (std::find(m_dataIndexes.cbegin(), m_dataIndexes.cend(), nextIndex) - m_dataIndexes.cbegin());
    unsigned swapDataIndex = m_dataIndexes[swapIndex] = m_dataIndexes[e];
    swap(m_comps[swapDataIndex], m_comps[nextIndex]);
    swap(m_depths[swapDataIndex], m_depths[nextIndex]);
    swap(m_order[swapDataIndex], m_order[nextIndex]);
}

SpatialComponent& ComponentContainer::getSpatialComponent(Entity e) {
    return m_comps[m_dataIndexes[e]];
}

int& ComponentContainer::getDepthComponent(Entity e) {
    return m_depths[m_dataIndexes[e]];
}

int& ComponentContainer::getOrderComponent(Entity e) {
    return m_order[m_dataIndexes[e]];
}

void ComponentContainer::forEachComponent(std::function<void(SpatialComponent&)> f) {
    std::for_each(m_comps.begin() + 1, m_comps.begin() + nextIndex, f);
}
