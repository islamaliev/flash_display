#pragma once

#include "Rectangle.h"
#include "Mat4.h"

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

        DisplayObject() = default;

        virtual ~DisplayObject() {};

        void setX(float value) { m_x = value; }
        float x() const { return m_x; }

        void setY(float value) { m_y = value; }
        float y() const { return m_y; }

        void setWidth(float value) { m_width = value; }
        float width() const { return m_width; }

        void setHeight(float value) { m_height = value; }
        float height() const { return m_height; }

        void setScaleX(float value) { m_scaleX = value; }
        float scaleX() const { return m_scaleX; }

        void setScaleY(float value) { m_scaleY = value; }
        float scaleY() const { return m_scaleY; }

        void setVisible(bool value) { m_visible = value; }
        bool visible() const { return m_visible; }

        void setPivotX(float value) { m_pivotX = value; }
        float pivotX() const { return m_pivotX; }

        void setPivotY(float value) { m_pivotY = value; }
        float pivotY() const { return m_pivotY; }

        void setRotation(float value) { m_rotation = value; }
        float rotation() const { return m_rotation; }

        virtual Rectangle getBounds(DisplayObject* targetSpace) const {
            return Rectangle(0, 0, m_width, m_height);
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

    private:
        void setParent(DisplayObjectContainer* parent) { m_parent = parent; }

        float m_x{0};
        float m_y{0};
        float m_width{1};
        float m_height{1};
        float m_scaleX{1};
        float m_scaleY{1};
        float m_pivotX{0};
        float m_pivotY{0};
        float m_rotation{0};
        bool m_visible{true};
        DisplayObjectContainer* m_parent{nullptr};

        friend class DisplayObjectContainer;
    };

}

}