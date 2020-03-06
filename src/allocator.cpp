#include <stl/allocator.hpp>

#include <new>

namespace stl {

void* allocator::allocate(size_t size) {
    if (size == 0) {
        return nullptr;
    }

    return ::operator new(size);
}

void allocator::deallocate(void* ptr, size_t size) {
    if (ptr == nullptr || size == 0) { return; }
    
    :: operator delete(ptr, size);
}

}