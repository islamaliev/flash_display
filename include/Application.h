#pragma once

namespace flash {
    
namespace ui {
    
    class Application {
    public:
        static Application& instance() {
            static Application app;
            return app;
        }
        
        int run();
        
    private:
        Application();
        
        friend class Window;
        
        void* m_data{nullptr};
    };
    
}
    
}
