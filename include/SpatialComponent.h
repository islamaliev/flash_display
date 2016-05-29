#pragma once


#include <vector>

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

    class SpatialComponentContainer {
    public:
        explicit SpatialComponentContainer(unsigned size)
                : m_comps(size + 1)
                , m_entities(size + 1)
                , m_dataIndexes(size + 1) {
        }

        const Entity& createIndex();

        void removeIndex(const Entity& index);

        SpatialComponent& getComponent(Entity index);

    private:
        unsigned nextIndex{1};

        std::vector<SpatialComponent> m_comps;
        std::vector<Entity> m_entities;
        std::vector<unsigned> m_dataIndexes;
        std::vector<unsigned> m_freeIndexes;
    };

}


