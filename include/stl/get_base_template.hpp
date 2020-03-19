#ifndef STL_GET_BASE_TEMPLATE_HPP_
#define STL_GET_BASE_TEMPLATE_HPP_

#include <stl/types.hpp>

namespace stl {

template<stl::size_t I, typename T>
auto& get(T& t) {
    return t.template _internal_get<I>();
}

template<stl::size_t I, typename T>
auto const& get(T const& t) {
    return t.template _internal_get<I>();
}

} // namespace stl

#endif