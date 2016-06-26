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
 *  container.forEach([](auto& c, int) { c.width += 1; });
 */

const Entity& ComponentContainer::createEntity() {
    assert(m_nextIndex < m_comps.size());
    unsigned entityIndex = m_nextIndex;
    if (m_freeEntities.size()) {
        entityIndex = m_freeEntities.back();
        m_freeEntities.pop_back();
    }
    Entity& entity = m_entities[entityIndex];
    new (&m_comps[m_nextIndex]) SpatialComponent();
    new (&m_depths[m_nextIndex]) int(-1);
    new (&m_backIndexes[m_nextIndex]) int(entityIndex);
    new (&m_order[entityIndex]) int(-1);
    m_dataIndexes[entityIndex] = m_nextIndex;
    ++m_nextIndex;
    entity = entityIndex;
    return entity;
}

void ComponentContainer::removeEntity(const Entity& e) {
    using std::swap;
    --m_nextIndex;
    m_freeEntities.push_back(e);
    int swapIndex = m_dataIndexes[m_nextIndex];
    int swapDataIndex = m_dataIndexes[swapIndex] = m_dataIndexes[e];
    m_comps[swapDataIndex] = m_comps[m_nextIndex];
    m_depths[swapDataIndex] = m_depths[m_nextIndex];
    m_backIndexes[swapDataIndex] = m_backIndexes[m_nextIndex];
    m_dataIndexes[e] = 0;
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

void ComponentContainer::forEach(std::function<void(SpatialComponent&, int)> f) {
    unsigned end = m_nextIndex - m_negativeOrdersNum;
    for (int i = 1; i < end; ++i) {
        f(m_comps[i], m_depths[i]);
    }
}

void ComponentContainer::sort() {
    using std::swap;
    unsigned i = 0;
    unsigned count = 1;
    m_negativeOrdersNum = 0;
    while (count < m_nextIndex) {
        ++i;
        if (m_dataIndexes[i] == 0) {
            continue;
        } else {
            int dataIndex = m_dataIndexes[i];
            int orderIndex = m_order[i] + 1;
            m_negativeOrdersNum += orderIndex == 0 ? 1 : 0;
            if (dataIndex != orderIndex && orderIndex != 0) {
                int backIndex = m_backIndexes[orderIndex];
                swap(m_comps[dataIndex], m_comps[orderIndex]);
                swap(m_depths[dataIndex], m_depths[orderIndex]);
                swap(m_backIndexes[dataIndex], m_backIndexes[orderIndex]);
                m_dataIndexes[i] = orderIndex;
                m_dataIndexes[backIndex] = dataIndex;
            }
            ++count;
        }
    }
}
