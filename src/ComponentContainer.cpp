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
    new (&m_backIndexes[nextIndex]) int(entityIndex);
    new (&m_order[entityIndex]) int(-1);
    m_dataIndexes[entityIndex] = nextIndex++;
    entity = entityIndex;
    return entity;
}

void ComponentContainer::removeEntity(const Entity& e) {
    using std::swap;
    --nextIndex;
    m_freeEntities.push_back(e);
    int swapIndex = m_dataIndexes[nextIndex];
    int swapDataIndex = m_dataIndexes[swapIndex] = m_dataIndexes[e];
    m_comps[swapDataIndex] = m_comps[nextIndex];
    m_depths[swapDataIndex] = m_depths[nextIndex];
    m_backIndexes[swapDataIndex] = m_backIndexes[nextIndex];
}

SpatialComponent& ComponentContainer::getSpatialComponent(Entity e) {
    return m_comps[m_dataIndexes[e]];
}

int& ComponentContainer::getDepthComponent(Entity e) {
    return m_depths[m_dataIndexes[e]];
}

int& ComponentContainer::getOrderComponent(Entity e) {
    return m_order[e];
}

void ComponentContainer::forEachComponent(std::function<void(SpatialComponent&)> f) {
    std::for_each(m_comps.begin() + 1, m_comps.begin() + nextIndex, f);
}

void ComponentContainer::sort() {
    unsigned i = 0;
    unsigned count = 1;
    while (count < nextIndex) {
        ++i;
        if (m_dataIndexes[i] == 0) {
            continue;
        } else {
            int dataIndex = m_dataIndexes[i];
            int orderIndex = m_order[i];
            if (dataIndex != orderIndex && orderIndex != -1) {
                int backIndex = m_backIndexes[orderIndex];
                m_comps[dataIndex] = m_comps[orderIndex];
                m_depths[dataIndex] = m_depths[orderIndex];
                m_backIndexes[dataIndex] = m_backIndexes[orderIndex];
                m_dataIndexes[i] = orderIndex;
                m_dataIndexes[backIndex] = dataIndex;
            }
            ++count;
        }
    }
}
