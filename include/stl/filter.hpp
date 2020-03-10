#ifndef STL_FILTER_HPP_
#define STL_FILTER_HPP_

#include <stl/iterator_traits.hpp>
#include <stl/memory.hpp>
#include <stl/types.hpp>

#include <functional>

namespace stl {

template<typename T>
class filter_view {
public:
    using compare_func_t = std::function<bool(T const&)>;

    class iterator {
    public:
        iterator() = default;

        iterator(compare_func_t* func, T* it);

        iterator(iterator const&) = default;
        iterator& operator=(iterator const&) = default;  

        iterator& operator++();
        iterator& operator++(int);

        bool operator==(iterator const& rhs) const;
        bool operator!=(iterator const& rhs) const;

        T& operator*();
        T const& operator*() const;

        stl::size_t operator-(iterator const& other) const;

    private:
        T* ptr = nullptr;
        compare_func_t* compare_func = nullptr;

        void advance();
    };

    template<typename It, typename F>
    filter_view(It begin, It end, F&& func);

    filter_view(filter_view const&) = default;
    filter_view& operator=(filter_view const&) = default;

    iterator begin();
    iterator end();

    stl::size_t size() const;

private:
    compare_func_t compare_func;

    iterator _begin;
    iterator _end;
};

template<typename T>
filter_view<T>::iterator::iterator(compare_func_t* func, T* it) : compare_func(func), ptr(it) {}

template<typename T>
typename filter_view<T>::iterator& filter_view<T>::iterator::operator++() {
    advance();
    return *this;
}

template<typename T>
typename filter_view<T>::iterator& filter_view<T>::iterator::operator++(int) {
    iterator copy = *this;
    advance();
    return copy;
}

template<typename T>
bool filter_view<T>::iterator::operator==(iterator const& rhs) const {
    return ptr == rhs.ptr;
}

template<typename T>
bool filter_view<T>::iterator::operator!=(iterator const& rhs) const {
    return ptr != rhs.ptr;
}

template<typename T>
T& filter_view<T>::iterator::operator*() {
    return *ptr;
}

template<typename T>
T const& filter_view<T>::iterator::operator*() const {
    return *ptr;
}

template<typename T>
stl::size_t filter_view<T>::iterator::operator-(iterator const& other) const {
    return ptr - other.ptr;
}

template<typename T>
void filter_view<T>::iterator::advance() {
    ++ptr;
    while(!(*compare_func)(*ptr)) {
        ++ptr;
    }
}


template<typename T>
template<typename It, typename F>
filter_view<T>::filter_view(It begin, It end, F&& func) : 
    compare_func(stl::forward<F>(func)),
    _begin(&this->compare_func, begin), _end(&this->compare_func, end) {

}

template<typename T>
typename filter_view<T>::iterator filter_view<T>::begin() {
    return _begin;
}

template<typename T>
typename filter_view<T>::iterator filter_view<T>::end() {
    return _end;
}

template<typename T>
stl::size_t filter_view<T>::size() const {
    return _end - _begin;
}

// Returns a filter_view with specified filter
template<typename InputIt, typename F, typename T = typename iterator_traits<InputIt>::value_type>
filter_view<T> filter(InputIt begin, InputIt end, F&& compare_func) {
    return filter_view<T>(begin, end, stl::forward<F>(compare_func));
}


} // namespace stl

#endif