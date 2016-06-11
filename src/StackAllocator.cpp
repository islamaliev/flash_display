#include <cstring>
#include <cassert>
#include "StackAllocator.h"

using namespace flash::allocator;
using Marker = StackAllocator::Marker;

StackAllocator::StackAllocator(size_t size)
    : m_size(size) {
    m_base = (char*) malloc(size);
}

StackAllocator::~StackAllocator() {
    free(m_base);
}

void* StackAllocator::alloc(size_t size) {
    char* p = m_base + m_top;
    assert(p - m_base + size <= m_size);
    memset(p, 0, size);
    m_top += size;
    return p;
}

const Marker StackAllocator::getMarker() const {
    return m_top;
}

void StackAllocator::freeToMarker(const Marker marker) {
    m_top = marker;
}

void StackAllocator::clear() {
    m_top = 0;
}