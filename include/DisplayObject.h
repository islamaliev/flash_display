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

        void setVisible(bool value) { m_visible = value; }
        bool visible() { return m_visible; }

    private:
        float m_x{0};
        float m_y{0};
        float m_width{1};
        float m_height{1};
        bool m_visible{true};
    };

}

}