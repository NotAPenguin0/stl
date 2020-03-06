#ifndef STL_VECTOR_HPP_
#define STL_VECTOR_HPP_

#include <stl/tags.hpp>
#include <stl/allocator.hpp>
#include <stl/memory.hpp>
#include <stl/algorithm.hpp>

namespace stl {

template<typename T, typename Allocator = stl::allocator>
class vector {
public:
    using value_type = T;
    using iterator = T*;
    using const_iterator = T const*;

    vector() = default;
    // Fills the vector with n default constructed items
    explicit vector(stl::size_t n);
    // Reserves space for n elements. Does not set the vector's size
    vector(stl::tags::reserve_tag, stl::size_t n);
    // Fills the vector with n uninitialized items
    vector(stl::tags::uninitialized_tag, stl::size_t n);
    // Fills the vector with n copies of initial_value
    vector(stl::size_t n, T const& initial_value);

    template<typename InputIt>
    vector(stl::tags::range_construct_tag, InputIt first, InputIt last);

    vector(vector const& other);
    vector(vector&& other);

    vector& operator=(vector const& other);
    vector& operator=(vector&& other);

    ~vector();

    T* data();
    T const* data() const;

    stl::size_t size() const;
    stl::size_t capacity() const;

    iterator begin();
    const_iterator begin() const;

    iterator end();
    const_iterator end() const;

    // Reserves new memory to use.
    void reserve(stl::size_t n);
    // Reserve new memory, and do not keep old data on reallocate
    void reserve(stl::size_t n, stl::tags::uninitialized_tag);
    // Resizes the vector and adds default constructed elements to the end
    void resize(stl::size_t n);
    // Resizes the vector and adds uninitialized elements at the end
    void resize(stl::size_t n, stl::tags::uninitialized_tag);

private:
    // Data
    Allocator _allocator;
    T* _data = nullptr;
    stl::size_t _size = 0;
    stl::size_t _capacity = 0;


    T* allocate(stl::size_t n);
    void deallocate(T* ptr, stl::size_t n);

    void reserve_uninitialized(stl::size_t n);
};

template<typename T, typename Allocator>
vector<T, Allocator>::vector(stl::size_t n) : vector(stl::tags::uninitialized, n) {
    // We have filled the vector with uninitialized elements. Now default construct them in-place in the vector
    inplace_construct_n(_data, n);
}

template<typename T, typename Allocator>
vector<T, Allocator>::vector(stl::tags::reserve_tag, stl::size_t n) {
    reserve_uninitialized(n);
}

template<typename T, typename Allocator>
vector<T, Allocator>::vector(stl::tags::uninitialized_tag, stl::size_t n) {
    reserve_uninitialized(n);
    _size = n;
}

template<typename T, typename Allocator>
vector<T, Allocator>::vector(stl::size_t n, T const& initial_value) : vector(stl::tags::uninitialized, n) {
    // The vector is filled with uninitialized data, now fill it with the initial values
    inplace_construct_n(_data, n, initial_value);
}

template<typename T, typename Allocator>
template<typename InputIt>
vector<T, Allocator>::vector(stl::tags::range_construct_tag, InputIt first, InputIt last) : 
    vector(stl::tags::uninitialized, stl::abs_distance(first, last)) {
    // Memory is reserved, now construct a new range
    inplace_construct_from_range(_data, first, last);
}

template<typename T, typename Allocator>
vector<T, Allocator>::vector(vector const& other) {
    _allocator = other._allocator;
    // Reserve memory and set correct size
    reserve_uninitialized(other._capacity);
    _size = other._size;
    // Copy elements
    inplace_construct_from_range(_data, other.begin(), other.end());
}

template<typename T, typename Allocator>
vector<T, Allocator>::vector(vector&& other) {
    _allocator = stl::move(other._allocator);
    _size = other._size;
    _capacity = other._capacity;
    _data = other._data;

    other._size = 0;
    other._capacity = 0;
    other._data = nullptr;
}

template<typename T, typename Allocator>
vector<T, Allocator>& vector<T, Allocator>::operator=(vector const& other) {
    // Self assignment check to avoid trouble
    if (this == &other) return *this;

    _allocator = other._allocator;

    // If we already have enough memory we do not need to reallocate
    if (_capacity >= other._size) {
        // Call destructor for previous data
        destruct_n(_data, _size);
        // Copy by constructing it in place from the other vector's range
        inplace_construct_from_range(_data, other.begin(), other.end());
        // Do not modify capacity here
        _size = other._size;
    } else {
        // We need a new allocation.

        // First, call the destructor for previous data
        destruct_n(_data, _size);
        // Free old memory
        deallocate(_data, _capacity);
        // Allocate new memory
        _data = allocate(other._capacity);
        // Copy over data from new vector
        inplace_construct_from_range(_data, other.begin(), other.end());
        _size = other._size;
        _capacity = other._capacity;
    }

    return *this;
}

template<typename T, typename Allocator>
vector<T, Allocator>& vector<T, Allocator>::operator=(vector&& other) {
    if (this == &other) return *this;

    _allocator = other._allocator;
    _size = other._size;
    _capacity = other._capacity;
    _data = other._data;

    other._size = 0;
    other._capacity = 0;
    other._data = nullptr;

    return *this;
}

template<typename T, typename Allocator>
vector<T, Allocator>::~vector() {
    // Call destructor for previous elements before freeing memory
    destruct_n(_data, _size);

    deallocate(_data, _capacity);
    _capacity = 0;
    _size = 0;
    _data = nullptr;
}

template<typename T, typename Allocator>
T* vector<T, Allocator>::data() {
    return _data;
}

template<typename T, typename Allocator>
T const* vector<T, Allocator>::data() const {
    return _data;
}

template<typename T, typename Allocator>
stl::size_t vector<T, Allocator>::size() const {
    return _size;
}

template<typename T, typename Allocator>
stl::size_t vector<T, Allocator>::capacity() const {
    return _capacity;
}

template<typename T, typename Allocator>
typename vector<T, Allocator>::iterator vector<T, Allocator>::begin() {
    return _data;
}

template<typename T, typename Allocator>
typename vector<T, Allocator>::const_iterator vector<T, Allocator>::begin() const {
    return _data;
}

template<typename T, typename Allocator>
typename vector<T, Allocator>::iterator vector<T, Allocator>::end() {
    return _data + _size;
}

template<typename T, typename Allocator>
typename vector<T, Allocator>::const_iterator vector<T, Allocator>::end() const {
    return _data + _size;
}

template<typename T, typename Allocator>
void vector<T, Allocator>::reserve(stl::size_t n) {
    // No need to allocate extra space
    if (_capacity >= n) { return; }

    // Allocate new memory
    T* new_data = allocate(n);
    // Move over old data
    inplace_move_from_range(new_data, begin(), end());
    // Free old memory
    destruct_n(_data, _size);
    deallocate(_data, _capacity);
    _capacity = n;
    _data = new_data;
}

template<typename T, typename Allocator>
void vector<T, Allocator>::reserve(stl::size_t n, stl::tags::uninitialized_tag) {
    if (_capacity >= n) { return; }

    destruct_n(_data, _size);
    deallocate(_data, _capacity);
    reserve_uninitialized(n);

    // With an uninitialized reserve, the size is set to zero to avoid access to uninitialized data
    _size = 0;
}

template<typename T, typename Allocator>
void vector<T, Allocator>::resize(stl::size_t n) {
    if (_size >= n) { return; }

    reserve(n);
    //  n - _size is the amount of elements appended to the vector. This call default constructs 
    // new elements at the end of the vector
    inplace_construct_n(end(), n - _size);
    _size = n;
    // _capacity is set inside reserve()
}

template<typename T, typename Allocator>
void vector<T, Allocator>::resize(stl::size_t n, stl::tags::uninitialized_tag) {
    if (_size >= n) { return; }

    reserve(n, stl::tags::uninitialized);
    _size = n;
    // _capacity is set inside reserve()
}

template<typename T, typename Allocator>
T* vector<T, Allocator>::allocate(stl::size_t n)  {
    return static_cast<T*>(_allocator.allocate(n * sizeof(T)));
}

template<typename T, typename Allocator>
void vector<T, Allocator>::deallocate(T* ptr, stl::size_t n) {
    _allocator.deallocate(ptr, n * sizeof(T));
}

template<typename T, typename Allocator>
void vector<T, Allocator>::reserve_uninitialized(stl::size_t n) {
    _data = allocate(n);
    _capacity = n;
}

} // namespace stl

#endif