#ifndef STL_ALLOCATOR_HPP_
#define STL_ALLOCATOR_HPP_

#include <stl/types.hpp>

namespace stl {

// Interface for allocator classes. Useful for polymorphic allocators
class allocator_base {
public:
    virtual ~allocator_base() = default;

    virtual void* allocate(stl::size_t size) = 0;
    virtual void deallocate(void* ptr, stl::size_t size) = 0;
};

class allocator : public allocator_base {
public:
    virtual ~allocator() = default;

    void* allocate(stl::size_t size) override;
    void deallocate(void* ptr, stl::size_t size) override;
};



}

#endif