#include "StackAllocator.h"
#include <cstring>
#include <cassert>
#include <cstdlib>

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
    assert(m_top + size <= m_size);
    char* p = m_base + m_top;
	m_top += static_cast<Marker>(size);
	return p;
}

const Marker StackAllocator::getMarker() const {
    return m_top;
}

void StackAllocator::freeToMarker(const Marker marker) {
    assert(marker <= m_top);
    m_top = marker;
}

void StackAllocator::clear() {
    m_top = 0;
}

void* StackAllocator::getPointer(Marker marker) const {
    return m_base + marker;
}