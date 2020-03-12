#ifndef STL_GET_BASE_TEMPLATE_HPP_
#define STL_GET_BASE_TEMPLATE_HPP_

#include <stl/types.hpp>

namespace stl {

template<typename T, stl::size_t N>
auto get(T& t) {
    return t.template _internal_get<N>();
}

template<typename T, stl::size_t N>
auto get(T const& t) {
    return t.template _internal_get<N>();
}

} // namespace stl

#endif