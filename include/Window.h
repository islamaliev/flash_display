#pragma once

namespace flash {
    
namespace ui {
    
    class Window {
    public:
        static Window* createWindow();
        
        ~Window();
        
        void close();
        
    private:
        Window() = default;
        
        void* m_data{nullptr};
    };
    
}
    
};
