#ifndef STL_SPAN_HPP_
#define STL_SPAN_HPP_

#include <stl/iterator_traits.hpp>
#include <stl/traits.hpp>

namespace stl {

template<typename T>
class span {
public:
    template<typename It>
    span(It begin, It end);

    template<typename Container>
    span(Container& c);

    span(span const&) = default;
    span& operator=(span const&) = default;

    T* begin();
    T const* begin() const;

    T* end();
    T const* end() const;

    stl::size_t size() const;

private:
    T* _begin;
    stl::size_t _size;
};

template<typename T>
template<typename It>
span<T>::span(It begin, It end) : _begin(begin), _size(end - begin) {

}

template<typename T>
template<typename Container>
span<T>::span(Container& c) : _begin(&*c.begin()), _size(c.size()) {

}

template<typename T>
T* span<T>::begin() {
    return _begin;
}

template<typename T>
T const* span<T>::begin() const {
    return _begin;
}

template<typename T>
T* span<T>::end() {
    return _begin + _size;
}

template<typename T>
T const* span<T>::end() const {
    return _begin + _size;
}

template<typename T>
stl::size_t span<T>::size() const {
    return _size;
}

template<typename Container>
span(Container& c) -> span<typename Container::value_type>;

template<typename It>
span(It begin, It end) -> span<typename iterator_traits<It>::value_type>;

} // namespace stl

#endif