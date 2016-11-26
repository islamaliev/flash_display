#pragma once

#include "DisplayObject.h"
#include <cstddef>
#include <vector>

namespace flash {

namespace display {

    class DisplayObjectContainer : public DisplayObject {
    public:
        DisplayObjectContainer();

        ~DisplayObjectContainer() {
            // TODO: set all nested method to noexcept
            removeChildren();
        }

        std::size_t numChildren() const { return m_children.size(); }

        void addChild(DisplayObject* child) {
            addChildAt(child, m_children.size());
        }

        void addChildAt(DisplayObject* child, unsigned index);

        void removeChild(DisplayObject* child) {
            _removeChildAt(std::find(m_children.cbegin(), m_children.cend(), child));
        }

        DisplayObject* removeChildAt(std::size_t index) {
            return _removeChildAt(m_children.cbegin() + index);
        }

        const DisplayObject* getChildAt(std::size_t index) const {
            return m_children[index];
        }

        DisplayObject* getChildAt(std::size_t index) {
            return m_children[index];
        }

        void removeChildren();
        
        void destroy() override;

        std::size_t getChildIndex(DisplayObject* child) const {
            const auto& beg = m_children.cbegin();
            return std::find(beg, m_children.cend(), child) - beg;
        }

        bool contains(DisplayObject* child) const {
            const auto& end = m_children.cend();
            return std::find(m_children.cbegin(), end, child) != end;
        }

        void preRender(flash::render::RenderState&) override;

        Rectangle getBounds(DisplayObject* targetSpace) const override;

        void setWidth(float value) override {};

        void setHeight(float value) override {};

    private:
        void _resetDepth() override;

        void _resetOrderIndex() override;

        void _updateDepth(int parentDepth) override;

        DisplayObject* _removeChildAt(std::vector<DisplayObject*>::const_iterator it);

        void _moveChildTo(DisplayObject* child, unsigned index);

        std::vector<DisplayObject*> m_children;

        friend class Stage;
    };

}

}