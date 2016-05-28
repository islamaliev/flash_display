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

        void setX(float value) { spatial().x = value; }
        float x() const { return spatial().x; }

        void setY(float value) { spatial().y = value; }
        float y() const { return spatial().y; }

        virtual void setWidth(float value);
        float width() const { return spatial().width; }

        virtual void setHeight(float value);
        float height() const { return spatial().height; }

        void setScaleX(float value);
        float scaleX() const { return spatial().scaleX; }

        void setScaleY(float value);
        float scaleY() const { return spatial().scaleY; }

        void setVisible(bool value) { m_visible = value; }
        bool visible() const { return m_visible; }

        void setPivotX(float value) { spatial().pivotX = value; }
        float pivotX() const { return spatial().pivotX; }

        void setPivotY(float value) { spatial().pivotY = value; }
        float pivotY() const { return spatial().pivotY; }

        void setRotation(float value) { m_rotation = value; }
        float rotation() const { return m_rotation; }

        virtual Rectangle getBounds(DisplayObject* targetSpace) const {
            return Rectangle(0, 0, spatial().width, spatial().height);
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

    protected:
        void setActualWidth(float value) {
            SpatialComponent& comp = spatial();
            comp.actualWidth = value;
            comp.width = value * comp.scaleX;
        }

        void setActualHeight(float value) {
            SpatialComponent& comp = spatial();
            comp.actualHeight = value;
            comp.height = value * comp.scaleY;
        }

        // TODO: get rid of these methods
        void _setWidth(float value) { spatial().width = value; }
        void _setHeight(float value) { spatial().height = value; }
        void _setScaleX(float value) { spatial().scaleX = value; }
        void _setScaleY(float value) { spatial().scaleY = value; }
        float _getActualWidth() { return spatial().actualWidth; }
        float _getActualHeight() { return spatial().actualHeight; }

    private:
        void setParent(DisplayObjectContainer* parent) { m_parent = parent; }

        SpatialComponent& spatial() { return const_cast<SpatialComponent&>(((const DisplayObject*) this)->spatial()); }
        const SpatialComponent& spatial() const;

        unsigned m_index{0};
        float m_rotation{0};
        bool m_visible{true};
        DisplayObjectContainer* m_parent{nullptr};

        friend class DisplayObjectContainer;
    };

}

}