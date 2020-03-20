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

// pack_element

namespace detail {

template<stl::size_t CurI, stl::size_t I, typename... Ts>
struct pack_element_impl;

// fallback for when there is only one element left to stop recursion
template<stl::size_t CurI, stl::size_t I, typename TFirst>
struct pack_element_impl<CurI, I, TFirst> {
    // This is always correct, since this only happens when we are at the end of the list
    using type = TFirst;
};

template<stl::size_t CurI, stl::size_t I, typename TFirst, typename TNext, typename... TRest>
struct pack_element_impl<CurI, I, TFirst, TNext, TRest...> {
    using type = stl::conditional_t<I == CurI, 
        TFirst, // Currently processed element in pack if we are at the matching index
        typename pack_element_impl<CurI + 1, I, TNext, TRest...>::type>; // Continue iterating if indices don't match
};

} // namespace detail

template<stl::size_t I, typename... Ts>
struct pack_element {
    static_assert(I < sizeof...(Ts), "pack_element index out of range");
    using type = typename detail::pack_element_impl<0, I, Ts...>::type;
};

// index_sequence and related

template<typename T, T... Is>
struct sequence {};

template<stl::size_t... Is>
struct index_sequence {};

namespace detail {

template<stl::size_t I, stl::size_t S, stl::size_t... Is>
struct make_index_sequence_impl {
    using type = typename make_index_sequence_impl<I + 1, S, Is..., I>::type;
};

// Stop recursion fallback
template<stl::size_t S, stl::size_t... Is>
struct make_index_sequence_impl<S, S, Is...> {
    using type = index_sequence<Is...>;
};

}

template<stl::size_t S>
using make_index_sequence = typename detail::make_index_sequence_impl<0, S>::type;

}

#endif