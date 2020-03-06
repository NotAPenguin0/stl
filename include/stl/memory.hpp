#ifndef STL_MEMORY_HPP_
#define STL_MEMORY_HPP_

#include <stl/types.hpp>
#include <stl/traits.hpp>

namespace stl {

template<typename T>
T&& forward(typename identity<T>::type&& param) { 
    return static_cast<typename identity<T>::type&&>(param); 
}

template<typename T>
T&& move(T& param) {
    return static_cast<T&&>(param);
}

template<typename OutputIt, typename... Args>
void inplace_construct_n(OutputIt begin, stl::size_t n, Args&&... args) {
    using T = remove_reference_t<decltype(*begin)>;

    while(n-- > 0) {
        new (begin++) T { stl::forward<Args>(args)... };
    }
}

template<typename OutputIt, typename... Args>
void inplace_construct_range(OutputIt begin, OutputIt end, Args&&... args) {
    using T = remove_reference_t<decltype(*begin)>;

    while(begin != end) {
        new (begin++) T { stl::forward<Args>(args)... };
    }
}

template<typename OutputIt, typename InputIt>
void inplace_construct_from_range(OutputIt begin, InputIt in_begin, InputIt in_end) {
    using T = remove_reference_t<decltype(*begin)>;

    while(in_begin != in_end) {
        new (begin++) T { *(in_begin++) };
    }
}

template<typename OutputIt, typename InputIt>
void inplace_move_from_range(OutputIt begin, InputIt in_begin, InputIt in_end) {
    using T = remove_reference_t<decltype(*begin)>;

    while(in_begin != in_end) {
        new (begin++) T { stl::move(*(in_begin++)) };
    }
}

template<typename OutputIt>
void destruct_n(OutputIt begin, stl::size_t n) {
    using T = remove_reference_t<decltype(*begin)>;

    while(n-- > 0) {
        (begin++)->~T();
    }
}

} // namespace stl

#endif