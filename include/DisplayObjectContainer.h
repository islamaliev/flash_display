#pragma once

#include "DisplayObject.h"
#include <cstddef>

namespace flash {

namespace display {

    class DisplayObjectContainer : public DisplayObject {
    public:
        std::size_t numChildren() { return m_numChildren; }

        void addChild(DisplayObject* child) {
            m_numChildren++;
        }

        void removeChild(DisplayObject* child) {
            m_numChildren--;
        }

    private:
        std::size_t m_numChildren{0};
    };

}

}