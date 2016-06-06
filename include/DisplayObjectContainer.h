#pragma once

#include "DisplayObject.h"
#include <cstddef>
#include <vector>

namespace flash {

namespace display {

    class DisplayObjectContainer : public DisplayObject {
    public:
        DisplayObjectContainer();

        std::size_t numChildren() const { return m_children.size(); }

        void addChild(DisplayObject* child) {
            m_children.push_back(child);
            child->_setParent(this);
            child->_updateDepth(depth());
            _alterTreeSizeBy(1);
        }

        void addChildAt(DisplayObject* child, std::size_t index) {
            auto it = m_children.begin();
            std::advance(it, index);
            m_children.insert(it, child);
            child->_setParent(this);
            child->_updateDepth(depth());
            _alterTreeSizeBy(1);
        }

        void removeChild(DisplayObject* child) {
            m_children.erase(std::remove(m_children.begin(), m_children.end(), child));
            child->_setParent(nullptr);
            child->_resetDepth();
            _alterTreeSizeBy(-1);
        }

        DisplayObject* removeChildAt(std::size_t index) {
            auto it = m_children.begin();
            std::advance(it, index);
            DisplayObject* result = *it;
            m_children.erase(it);
            result->_setParent(nullptr);
            result->_resetDepth();
            _alterTreeSizeBy(-1);
            return result;
        }

        const DisplayObject* getChildAt(std::size_t index) const {
            return m_children[index];
        }

        DisplayObject* getChildAt(std::size_t index) {
            return m_children[index];
        }

        void removeChildren() {
            DisplayObject::_resetDepth();
            _setTreeSize(1);
            for (auto& child : m_children) {
                child->_resetDepth();
                child->_setParent(nullptr);
            }
            m_children.clear();
        }

        std::size_t getChildIndex(DisplayObject* child) const {
            auto beg = m_children.cbegin();
            return std::find(beg, m_children.cend(), child) - beg;
        }

        bool contains(DisplayObject* child) const {
            return std::find(m_children.cbegin(), m_children.cend(), child) != m_children.cend();
        }

        void draw(flash::render::Context&, flash::render::RenderState&) override;

        Rectangle getBounds(DisplayObject* targetSpace) const override;

        void setWidth(float value) override {};

        void setHeight(float value) override {};

    private:
        void _resetDepth() override {
            DisplayObject::_resetDepth();
            for (auto& child : m_children) {
                child->_resetDepth();
            }
        }

        void _updateDepth(int parentDepth) override {
            DisplayObject::_updateDepth(parentDepth);
            int d = depth();
            for (auto& child : m_children) {
                child->_updateDepth(d);
            }
        }

    protected:
        std::vector<DisplayObject*> m_children;
    };

}

}