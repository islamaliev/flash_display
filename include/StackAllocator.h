#pragma once


namespace flash {

namespace allocator {

    class StackAllocator {
    public:
        using Marker = size_t;

        explicit StackAllocator(size_t size);

        ~StackAllocator();

        void* alloc(size_t size);

        const Marker getMarker() const;

        void freeToMarker(const Marker marker);

        void clear();

    private:
        Marker m_top{0};
        size_t m_size{0};
        char* m_base{nullptr};
    };

}

}
