#ifndef STL_UTILITY_HPP_
#define STL_UTILITY_HPP_

#include <stl/traits.hpp>

namespace stl {

template<typename T>
constexpr T&& forward(stl::remove_reference_t<T>& param) noexcept {
    return static_cast<T&&>(param);
}

template<typename T>
constexpr T&& forward(stl::remove_reference_t<T>&& param) noexcept {
    return static_cast<T&&>(param);
}

template<typename T>
constexpr T&& move(T& param) {
    return static_cast<T&&>(param);
}

}

#endif