#ifndef STL_ENUMERATE_HPP_
#define STL_ENUMERATE_HPP_

#include <stl/algorithm.hpp>
#include <stl/iterator_traits.hpp>
#include <stl/types.hpp>
#include <stl/get_base_template.hpp>

namespace stl {

template<typename T>
class enumerate_view {
public:
    struct value_type {
    private:
        template<stl::size_t N>
        struct return_type { using type = void; };

        template<>
        struct return_type<0> { using type = stl::size_t; };

        template<>
        struct return_type<1> { using type = T&; };
    public:
        T& value;
        stl::size_t index = 0;

        template<stl::size_t N>
        typename return_type<N>::type _internal_get() {}

        template<>
        stl::size_t _internal_get<0>() {
            return index;
        }

        template<>
        T& _internal_get<1>() {
            return value;
        }
    };

    struct const_value_type {
    private:
        template<stl::size_t N>
        struct return_type { using type = void; };

        template<>
        struct return_type<0> { using type = stl::size_t; };

        template<>
        struct return_type<1> { using type = T const&; };
    public:
        T const& value;
        stl::size_t index = 0;

        template<stl::size_t N>
        typename return_type<N>::type _internal_get() const {}

        template<>
        stl::size_t _internal_get<0>() const {
            return index;
        }

        template<>
        T const& _internal_get<1>() const {
            return value;
        }
    };

    class iterator {
    public:
        iterator() = default;
        iterator(T* ptr, stl::size_t index);

        iterator(iterator const&) = default;
        iterator& operator=(iterator const&) = default;

        iterator& operator++();
        iterator operator++(int);

        bool operator==(iterator const& rhs) const;
        bool operator!=(iterator const& rhs) const;

        value_type operator*();
        const_value_type operator*() const;
        
    private:
        T* _ptr = nullptr;
        stl::size_t _index = 0;
    };

    template<typename It>
    enumerate_view(It begin, It end);

    enumerate_view(enumerate_view const&) = default;
    enumerate_view& operator=(enumerate_view const&) = default;

    iterator begin();
    iterator end();

    stl::size_t size() const;

private:
    T* _begin;
    stl::size_t _size;
};

template<typename T>
enumerate_view<T>::iterator::iterator(T* ptr, stl::size_t index) {
    _ptr = ptr;
    _index = index;
}

template<typename T>
typename enumerate_view<T>::iterator& enumerate_view<T>::iterator::operator++() {
    ++_ptr;
    ++_index;
    return *this;
}

template<typename T>
typename enumerate_view<T>::iterator enumerate_view<T>::iterator::operator++(int) {
    iterator copy = *this;
    ++_ptr;
    ++_index;
    return copy;
}

template<typename T>
bool enumerate_view<T>::iterator::operator==(iterator const& rhs) const {
    return _ptr == rhs._ptr && _index == rhs._index;
}

template<typename T>
bool enumerate_view<T>::iterator::operator!=(iterator const& rhs) const {
    return _ptr != rhs._ptr && _index != rhs._index;
}

template<typename T>
typename enumerate_view<T>::value_type enumerate_view<T>::iterator::operator*() {
    return { *_ptr, _index };
}

template<typename T>
typename enumerate_view<T>::const_value_type enumerate_view<T>::iterator::operator*() const {
    return { *_ptr, _index };
}

template<typename T>
template<typename It>
enumerate_view<T>::enumerate_view(It begin, It end) {
    _begin = begin;
    _size = stl::abs_distance(begin, end);
}

template<typename T>
typename enumerate_view<T>::iterator enumerate_view<T>::begin() {
    return iterator(_begin, 0);
}

template<typename T>
typename enumerate_view<T>::iterator enumerate_view<T>::end() {
    return iterator(_begin + _size, _size);
}

template<typename T>
stl::size_t enumerate_view<T>::size() const {
    return _size;
}

template<typename It, typename T = typename iterator_traits<It>::value_type>
enumerate_view<T> enumerate(It begin, It end) {
    return enumerate_view<T>(begin, end);
}

} // namespace stl

#endif