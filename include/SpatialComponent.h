#pragma once


#include <vector>

namespace flash {

    class Entity {
    public:
        bool operator==(const Entity& other) const {
            return m_index == other.m_index;
        }

    private:
        unsigned m_dataIndex{0};
        unsigned m_index{0};

        friend class SpatialComponentContainer;
    };

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
                , m_entities(size + 1) {
        }

        const Entity& createIndex();

        void removeIndex(const Entity& index);

        SpatialComponent& getComponent(Entity index);

    private:
        unsigned nextIndex{1};

        std::vector<unsigned> m_freeIndexes;
        std::vector<SpatialComponent> m_comps;
        std::vector<Entity> m_entities;
    };

}


