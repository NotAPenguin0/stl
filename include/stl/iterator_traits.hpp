#ifndef STL_ITERATOR_TRAITS_HPP_
#define STL_ITERATOR_TRAITS_HPP_

namespace stl {

template<typename T>
struct iterator_traits;

template<typename T>
struct iterator_traits<T*> {
    using value_type = T;
};

}

#endif