#ifndef SATURN_STL_ALGORITHM_HPP_
#define SATURN_STL_ALGORITHM_HPP_

#include <stl/types.hpp>

#include <cmath>

namespace stl {

template<typename T, typename U>
T min(T const& a, U const& b) {
    return a < b ? a : b;
}

template<typename T, typename U>
T max(T const& a, U const& b) {
    return b < a ? a : b;
}

template<typename InputIt>
stl::size_t abs_distance(InputIt first, InputIt last) {
    return std::abs(last - first);
}

}

#endif