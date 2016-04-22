namespace flash {

namespace display {

    class DisplayObject {
    public:
        DisplayObject();

        void setWidth(float value) { m_width = value; }
        float width() { return m_width; }

        void setHeight(float value) { m_height = value; }
        float height() { return m_height; }

        void setVisible(bool value) { m_visible = value; }
        bool visible() { return m_visible; }

    private:
        float m_width{0};
        float m_height{0};
        bool m_visible{true};
    };

}

}