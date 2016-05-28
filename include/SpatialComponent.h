#pragma once


#include <vector>

namespace flash {

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
        : comps(size) {
    }

    unsigned createIndex();

    void removeIndex(unsigned index);

    SpatialComponent& getComponent(unsigned index);

private:
    unsigned nextIndex;

    std::vector<SpatialComponent> comps;
};

}


