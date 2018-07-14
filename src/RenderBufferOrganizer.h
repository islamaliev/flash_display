#pragma once

namespace flash {

class ComponentContainer;
    
namespace allocator {
    class StackAllocator;
}
    
namespace display {
    class DisplayObject;
}
    
namespace render {
    struct BufferData;

    class RenderBufferOrganizer {
    public:
        static void organize(flash::display::DisplayObject&, allocator::StackAllocator&, BufferData&);
    };

}

}

