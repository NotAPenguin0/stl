#ifndef STL_TUPLE_HPP_
#define STL_TUPLE_HPP_

#include <stl/get_base_template.hpp>
#include <stl/traits.hpp>
#include <stl/types.hpp>
#include <stl/utility.hpp>

namespace stl {

namespace detail {

// tuple_element for non-reference types
template<stl::size_t I, typename T>
struct tuple_element {
    tuple_element() = default;
    tuple_element(tuple_element const&) = default;
    tuple_element(tuple_element&&) = default;

    tuple_element& operator=(tuple_element const&) = default;
    tuple_element& operator=(tuple_element&&) = default;

    tuple_element(T const& value) : value(value) {}

    tuple_element(T&& value) : value(stl::move(value)) {}

    T value;

    T& get_value() {
        return value;
    }

    T const& get_value() const {
        return value;
    }
};


// tuple_element for reference types
template<stl::size_t I, typename T>
struct tuple_element<I, T&> {
    tuple_element(tuple_element const&) = default;
    tuple_element(tuple_element&&) = default;

    tuple_element(T& value) : value(value) {}

    T& value;

    T& get_value() {
        return value;
    }

    T const& get_value() const {
        return value;
    }
};

// tuple_element for const reference types
template<stl::size_t I, typename T>
struct tuple_element<I, T const&> {
    tuple_element(tuple_element const&) = default;
    tuple_element(tuple_element&&) = default;

    tuple_element(T const& value) : value(value) {}

    T const& value;

    T const& get_value() const {
        return value;
    }
};

template<typename Is, typename... Ts>
struct tuple_storage_base;

template<stl::size_t... Is, typename... Ts>
struct tuple_storage_base<stl::index_sequence<Is...>, Ts...> : public tuple_element<Is, Ts> ... {

    tuple_storage_base() = default;
    tuple_storage_base(tuple_storage_base const&) = default;
    tuple_storage_base(tuple_storage_base&&) = default;

    tuple_storage_base& operator=(tuple_storage_base const&) = default;
    tuple_storage_base& operator=(tuple_storage_base&&) = default;

    template<typename... Us>
    tuple_storage_base(Us&&... values) : tuple_element<Is, Ts>(stl::forward<Us>(values)) ... {
        
    }

    template<stl::size_t I>
    auto& _internal_get() {
        using value_type = typename pack_element<I, Ts...>::type;
        return detail::tuple_element<I, value_type>::get_value();
    }

    template<stl::size_t I>
    auto const& _internal_get() const {
        using value_type = typename pack_element<I, Ts...>::type;
        return detail::tuple_element<I, value_type>::get_value();
    }
};

template<typename... Ts>
struct tuple_storage : tuple_storage_base<stl::make_index_sequence<sizeof...(Ts)>, Ts...> {
    using tuple_storage_base<stl::make_index_sequence<sizeof...(Ts)>, Ts...>::tuple_storage_base;
};

} // namespace detail

template<typename... Ts>
class tuple : public detail::tuple_storage<Ts...> {
public:
    tuple() = default;

    template<typename... Us, typename Enable = std::enable_if_t<sizeof...(Us) == sizeof...(Ts)>>
    tuple(Us&&... values) : detail::tuple_storage<Ts...>(stl::forward<Us>(values) ...) {
        
    }

    tuple(tuple const& rhs) = default;
    tuple(tuple&&) = default;

    tuple& operator=(tuple const&) = default;
    tuple& operator=(tuple&&) = default;

    template<stl::size_t I>
    auto& _internal_get() {
        return detail::tuple_storage<Ts...>::template _internal_get<I>();
    }

    template<stl::size_t I>
    auto const& _internal_get() const {
        return detail::tuple_storage<Ts...>::template _internal_get<I>();
    }
};

template<typename... Ts>
tuple<stl::remove_reference_t<Ts>...> make_tuple(Ts&&... values) {
    return tuple<stl::remove_reference_t<Ts>...>(stl::forward<Ts>(values) ...);
}

template<typename... Ts>
tuple<Ts& ...> tie(Ts& ... values) {
    return tuple<Ts& ...>(stl::forward<Ts&>(values) ...);
}

namespace detail {

template<typename... Ts, typename... Us, stl::size_t... Is1, stl::size_t... Is2>
tuple<Ts..., Us...> concat_two_tuples_impl(tuple<Ts...> lhs, tuple<Us...> rhs, 
    index_sequence<Is1...>, index_sequence<Is2...>) {

    return tuple<Ts..., Us...>(get<Is1>(lhs) ..., get<Is2>(rhs) ...);
}

template<typename... Ts, typename... Us>
tuple<Ts..., Us...> concat_two_tuples(tuple<Ts...> lhs, tuple<Us...> rhs) {
    return concat_two_tuples_impl(stl::move(lhs), stl::move(rhs), 
        make_index_sequence<sizeof...(Ts)>{}, 
        make_index_sequence<sizeof...(Us)>{});
}

// Fallback for when only one tuple is left
template<typename... Ts>
auto tuple_cat_impl(tuple<Ts...> tpl) {
    return tpl;
}

template<typename... Ts, typename... Us, typename... Tuples>
auto tuple_cat_impl(tuple<Ts...> lhs, tuple<Us...> rhs, Tuples&&... tuples) {
    // Concatenate lhs and rhs, then combine result with the rest of the recursion
    return tuple_cat_impl(
        concat_two_tuples(stl::move(lhs), stl::move(rhs)),
        stl::forward<Tuples>(tuples) ...
    );
}

}


template<typename... Tuple>
auto tuple_cat(Tuple&&... tuples) {
    return detail::tuple_cat_impl(stl::forward<Tuple>(tuples) ...);
}

} // namespace stl

// Necessary to make structured bindings work
namespace std {
template<typename Tuple>
struct tuple_size;

template<typename... Ts>
struct tuple_size<stl::tuple<Ts...>> {
    static constexpr stl::size_t value = sizeof...(Ts);
};

template<stl::size_t I, typename Tuple>
struct tuple_element;

template<stl::size_t I, typename... Ts>
struct tuple_element<I, stl::tuple<Ts...>> {
    using type = typename stl::pack_element<I, Ts...>::type;
};
} // namespace std

namespace stl {

template<typename Tuple>
using tuple_size = std::tuple_size<Tuple>;

template<typename Tuple>
constexpr stl::size_t tuple_size_v = tuple_size<Tuple>::value;

template<stl::size_t I, typename Tuple>
using tuple_element = std::tuple_element<I, Tuple>;

template<stl::size_t I, typename Tuple>
using tuple_element_t = typename tuple_element<I, Tuple>::type;

} // namespace std

#endif