#pragma once


#include <vector>
#include <functional>

namespace flash {

    using Entity = unsigned;

    struct SpatialComponent {
        static constexpr unsigned DEFAULT_SIZE = 40;

        float width{DEFAULT_SIZE};
        float height{DEFAULT_SIZE};
        float scaleX{1};
        float scaleY{1};
        float x{0};
        float y{0};
        float pivotX{0};
        float pivotY{0};
    };

    struct TextureData {
        unsigned color{0};
        int textureId{0};
    };

    class ComponentContainer {
    public:
        explicit ComponentContainer(unsigned size)
                : m_comps(size + 1)
                , m_depths(size + 1)
                , m_textures(size + 1)
                , m_backIndexes(size + 1)
                , m_order(size + 1)
                , m_entities(size + 1)
                , m_dataIndexes(size + 1, 0) {
        }

        const Entity& createEntity();

        void removeEntity(const Entity&);

        void forEach(std::function<void(SpatialComponent&, TextureData&, int, int)> f);

        void forEachTextureData(std::function<void(TextureData&, int)>);

        SpatialComponent& getSpatialComponent(Entity);

        int& getDepthComponent(Entity);

        TextureData& getTextureData(Entity);

        int& getOrderComponent(Entity);

        void sort();
        
        void clear();

    private:
        unsigned m_nextIndex{1};
        unsigned m_negativeOrdersNum{0};
        
        // TODO: use one big memory buffer instead several vectors
        std::vector<SpatialComponent> m_comps;
        std::vector<int> m_depths;
        std::vector<TextureData> m_textures;
        std::vector<int> m_backIndexes;
        std::vector<int> m_order;
        std::vector<Entity> m_entities;
        std::vector<int> m_dataIndexes;
        std::vector<unsigned> m_freeEntities;
    };

}


