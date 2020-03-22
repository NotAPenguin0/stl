#ifndef STL_ENUMERATE_HPP_
#define STL_ENUMERATE_HPP_

#include <stl/algorithm.hpp>
#include <stl/iterator_traits.hpp>
#include <stl/types.hpp>
#include <stl/get_base_template.hpp>

namespace stl {

namespace detail {

template<typename T>
struct enumerate_value_type {
    stl::size_t index;
    T& value;
};

template<typename T>
struct const_enumerate_value_type {
    stl::size_t _index = 0;
    T const& _value;
};

} // namespace detail

template<typename T>
class enumerate_view {
public:  
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

        detail::enumerate_value_type<T> operator*();
        detail::const_enumerate_value_type<T> operator*() const;
        
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
detail::enumerate_value_type<T> enumerate_view<T>::iterator::operator*() {
    return { _index, *_ptr };
}

template<typename T>
detail::const_enumerate_value_type<T> enumerate_view<T>::iterator::operator*() const {
    return { _index, *_ptr };
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