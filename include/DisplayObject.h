#pragma once

#include "Rectangle.h"
#include "Mat4.h"
#include "SpatialComponent.h"

namespace flash {

namespace render {
    class Context;
    class RenderState;
}

namespace display {

    class DisplayObjectContainer;

    class DisplayObject {
    public:

        using Rectangle = flash::core::Rectangle;

        DisplayObject();

        virtual ~DisplayObject() {};

        void setX(float value) { _spatial().x = value; }
        float x() const { return _spatial().x; }

        void setY(float value) { _spatial().y = value; }
        float y() const { return _spatial().y; }

        virtual void setWidth(float value);
        float width() const { return _spatial().width; }

        virtual void setHeight(float value);
        float height() const { return _spatial().height; }

        void setScaleX(float value);
        float scaleX() const { return _spatial().scaleX; }

        void setScaleY(float value);
        float scaleY() const { return _spatial().scaleY; }

        void setVisible(bool value) { m_visible = value; }
        bool visible() const { return m_visible; }

        void setPivotX(float value) { _spatial().pivotX = value; }
        float pivotX() const { return _spatial().pivotX; }

        void setPivotY(float value) { _spatial().pivotY = value; }
        float pivotY() const { return _spatial().pivotY; }

        void setRotation(float value) { m_rotation = value; }
        float rotation() const { return m_rotation; }

        virtual Rectangle getBounds(DisplayObject* targetSpace) const {
            return Rectangle(0, 0, _spatial().width, _spatial().height);
        }

        virtual void draw(flash::render::Context&, flash::render::RenderState&);

        flash::math::Mat4 getTransform() const;

        flash::math::Mat4 getTransform(DisplayObjectContainer* targetSpace) const;

        const DisplayObjectContainer* getParent() const {
            return m_parent;
        }

        DisplayObjectContainer* getParent() {
            return m_parent;
        }

        int depth() const;

        int treeSize() const { return m_treeSize; }

        int orderIndex() const;

    protected:
        void setActualWidth(float value) {
            SpatialComponent& comp = _spatial();
            comp.actualWidth = value;
            comp.width = value * comp.scaleX;
        }

        void setActualHeight(float value) {
            SpatialComponent& comp = _spatial();
            comp.actualHeight = value;
            comp.height = value * comp.scaleY;
        }

        // TODO: get rid of these methods
        void _setWidth(float value) { _spatial().width = value; }
        void _setHeight(float value) { _spatial().height = value; }
        void _setScaleX(float value) { _spatial().scaleX = value; }
        void _setScaleY(float value) { _spatial().scaleY = value; }
        float _getActualWidth() { return _spatial().actualWidth; }
        float _getActualHeight() { return _spatial().actualHeight; }

    private:
        virtual void _resetDepth() { _setDepth(-1); };

        virtual void _updateDepth(int parentDepth) { _setDepth(parentDepth != -1 ? parentDepth + 1 : -1); };

        void _setParent(DisplayObjectContainer* parent) { m_parent = parent; }

        void _setDepth(int value);

        void _setTreeSize(int value) { m_treeSize = value; }

        void _alterTreeSizeBy(int value);

        virtual void _updateOrderIndex(int& orderIndex);

        SpatialComponent& _spatial() { return const_cast<SpatialComponent&>(((const DisplayObject*) this)->_spatial()); }
        const SpatialComponent& _spatial() const;

        Entity m_index;
        float m_rotation{0};
        int m_treeSize{1};
        bool m_visible{true};
        DisplayObjectContainer* m_parent{nullptr};

        friend class DisplayObjectContainer;
        friend class Stage;
    };

}

}