#pragma once

namespace flash 
{
    
namespace ui 
{
    
    class Application 
	{
    public:
        static Application& instance() 
		{
            static Application app;
            return app;
        }
        
		void init(int width, int height, void(*onInitDone)(void));

        int run(void(*updateF)(void));
        
        bool isRunning() const;
        
        void stop();
        
        void* m_data{nullptr};
        
    private:
        Application();
    };
}
    
}
