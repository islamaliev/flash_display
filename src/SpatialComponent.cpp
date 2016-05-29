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
    entity.m_dataIndex = nextIndex++;
    entity.m_index = entityIndex;
    return entity;
}

void SpatialComponentContainer::removeIndex(const Entity& index) {
    using std::swap;
    --nextIndex;
    m_freeIndexes.push_back(index.m_index);
    Entity& swapEntity = *std::find_if(m_entities.begin(), m_entities.end(), [=](auto& e) {
        return e.m_dataIndex == nextIndex;
    });
    swapEntity.m_dataIndex = index.m_dataIndex;
    swap(m_comps[swapEntity.m_dataIndex], m_comps[nextIndex]);
}

SpatialComponent& SpatialComponentContainer::getComponent(Entity index) {
    return m_comps[index.m_dataIndex];
}
