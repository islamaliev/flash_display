#pragma once


#include <vector>
#include <functional>

namespace flash {

    using Entity = unsigned;

    struct SpatialComponent {
        static constexpr unsigned DEFAULT_SIZE = 40;

        float width{DEFAULT_SIZE};
        float height{DEFAULT_SIZE};
        // TODO: get rid of unnecessary fields
        float actualWidth{DEFAULT_SIZE};
        float actualHeight{DEFAULT_SIZE};
        float scaleX{1};
        float scaleY{1};
        float x{0};
        float y{0};
        float pivotX{0};
        float pivotY{0};
    };

    class ComponentContainer {
    public:
        explicit ComponentContainer(unsigned size)
                : m_comps(size + 1)
                , m_depths(size + 1)
                , m_order(size + 1)
                , m_entities(size + 1)
                , m_dataIndexes(size + 1) {
        }

        const Entity& createEntity();

        void removeEntity(const Entity&);

        void forEachComponent(std::function<void(SpatialComponent&)>);

        SpatialComponent& getSpatialComponent(Entity);

        int& getDepthComponent(Entity);

        int& getOrderComponent(Entity);

    private:
        unsigned nextIndex{1};

        std::vector<SpatialComponent> m_comps;
        std::vector<int> m_depths;
        std::vector<int> m_order;
        std::vector<Entity> m_entities;
        std::vector<unsigned> m_dataIndexes;
        std::vector<unsigned> m_freeEntities;
    };

}


