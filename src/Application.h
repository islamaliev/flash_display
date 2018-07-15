#pragma once

namespace flash {
    
namespace ui {
    
    class Application {
    public:
        static Application& instance() {
            static Application app;
            return app;
        }
        
        int run(void(*initF)(void), void(*updateF)(void));
        
        bool isRunning() const;
        
        void stop();
        
        void swap();
        
        void* m_data{nullptr};
        
    private:
        Application();
    };
}
    
}
