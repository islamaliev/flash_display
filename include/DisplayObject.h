namespace flash {

namespace display {

    class DisplayObject {
    public:
        DisplayObject();

        void setX(float value) { m_x = value; }
        float x() { return m_x; }

        void setY(float value) { m_y = value; }
        float y() { return m_y; }

        void setWidth(float value) { m_width = value; }
        float width() { return m_width; }

        void setHeight(float value) { m_height = value; }
        float height() { return m_height; }

        void setScaleX(float value) { m_scaleX = value; }
        float scaleX() { return m_scaleX; }

        void setScaleY(float value) { m_scaleY = value; }
        float scaleY() { return m_scaleY; }

        void setVisible(bool value) { m_visible = value; }
        bool visible() { return m_visible; }

        void setPivotX(float value) { m_pivotX = value; }
        float pivotX() { return m_pivotX; }

        void setPivotY(float value) { m_pivotY = value; }
        float pivotY() { return m_pivotY; }

        void setRotation(float value) { m_rotation = value; }
        float rotation() { return m_rotation; }

    private:
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
    };

}

}