#pragma once


namespace flash {

namespace allocator {

    class StackAllocator {
    public:
        using Marker = unsigned;

        explicit StackAllocator(size_t);

        ~StackAllocator();

        void* alloc(size_t);

        void* getPointer(Marker) const;

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
