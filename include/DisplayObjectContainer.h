#pragma once

#include "DisplayObject.h"
#include <cstddef>
#include <vector>

namespace flash {

namespace display {

    class DisplayObjectContainer : public DisplayObject {
    public:
        std::size_t numChildren() const { return m_children.size(); }

        void addChild(DisplayObject* child) {
            m_children.push_back(child);
            child->setParent(this);
        }

        void addChildAt(DisplayObject* child, std::size_t index) {
            auto it = m_children.begin();
            std::advance(it, index);
            m_children.insert(it, child);
            child->setParent(this);
        }

        void removeChild(DisplayObject* child) {
            m_children.erase(std::remove(m_children.begin(), m_children.end(), child));
            child->setParent(nullptr);
        }

        DisplayObject* removeChildAt(std::size_t index) {
            auto it = m_children.begin();
            std::advance(it, index);
            DisplayObject* result = *it;
            m_children.erase(it);
            result->setParent(nullptr);
            return result;
        }

        const DisplayObject* getChildAt(std::size_t index) const {
            return m_children[index];
        }

        DisplayObject* getChildAt(std::size_t index) {
            return m_children[index];
        }

        void removeChildren() {
            for (auto& child : m_children) {
                child->setParent(nullptr);
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

        void draw(flash::render::Context& context) override;

        Rectangle getBounds(DisplayObject* targetSpace) const override;

    private:
        std::vector<DisplayObject*> m_children;
    };

}

}