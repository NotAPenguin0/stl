#ifndef STL_VECTOR_HPP_
#define STL_VECTOR_HPP_

#include <stl/tags.hpp>
#include <stl/allocator.hpp>
#include <stl/memory.hpp>
#include <stl/utility.hpp>
#include <stl/algorithm.hpp>
#include <stl/assert.hpp>
#include <stl/exception.hpp>

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

    bool empty() const;

    iterator begin();
    const_iterator begin() const;

    iterator end();
    const_iterator end() const;

    T& operator[](stl::size_t i);
    T const& operator[](stl::size_t i) const;

    T& at(stl::size_t i);
    T const& at(stl::size_t i) const;

    T& front();
    T const& front() const;

    T& back();
    T const& back() const;

    // Reserves new memory to use.
    void reserve(stl::size_t n);
    // Reserve new memory, and do not keep old data on reallocate
    void reserve(stl::tags::uninitialized_tag, stl::size_t n);
    // Resizes the vector and adds default constructed elements to the end
    void resize(stl::size_t n);
    // Resizes the vector and adds uninitialized elements at the end
    void resize(stl::tags::uninitialized_tag, stl::size_t n);

    void push_back(T const& value);
    void push_back(T&& value);

    template<typename... Args>
    void emplace_back(Args&&... args);

    void clear();
    void shrink_to_fit();

    // Inserts value before pos. Returns the iterator pointing to the inserted value
    iterator insert(iterator pos, T const& value);
    iterator insert(iterator pos, T&& value);

    // Erases the value at iterator pos. Returns the iterator pointing to the value next to it.
    iterator erase(iterator pos);

private:
    // Data
    Allocator _allocator;
    T* _data = nullptr;
    stl::size_t _size = 0;
    stl::size_t _capacity = 0;


    T* allocate(stl::size_t n);
    void deallocate(T* ptr, stl::size_t n);

    void reserve_uninitialized(stl::size_t n);
    stl::size_t calc_grow_size() const;
    // Grows the vector so it can hold n elements. Moves over old data
    void grow(stl::size_t n);
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
bool vector<T, Allocator>::empty() const {
    return _size == 0;
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
T& vector<T, Allocator>::operator[](stl::size_t i) {
    STL_ASSERT(i < _size, "vector index out of range");
    return _data[i];
}

template<typename T, typename Allocator>
T const& vector<T, Allocator>::operator[](stl::size_t i) const {
    STL_ASSERT(i < _size, "vector index out of range");
    return _data[i];
}

template<typename T, typename Allocator>
T& vector<T, Allocator>::at(stl::size_t i) {
    if (i >= _size) throw std::out_of_range("vector index out of range");
    return _data[i];
}

template<typename T, typename Allocator>
T const& vector<T, Allocator>::at(stl::size_t i) const {
    if (i >= _size) throw std::out_of_range("vector index out of range");
    return _data[i];
}

template<typename T, typename Allocator>
T& vector<T, Allocator>::front() {
    STL_ASSERT(!empty(), "front() called on empty vector");
    return _data[0];
}

template<typename T, typename Allocator>
T const& vector<T, Allocator>::front() const {
    STL_ASSERT(!empty(), "front() called on empty vector");
    return _data[0];
}

template<typename T, typename Allocator>
T& vector<T, Allocator>::back() {
    STL_ASSERT(!empty(), "back() called on empty vector");
    return _data[_size - 1];
}

template<typename T, typename Allocator>
T const& vector<T, Allocator>::back() const {
    STL_ASSERT(!empty(), "back() called on empty vector");
    return _data[_size - 1];
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
void vector<T, Allocator>::reserve(stl::tags::uninitialized_tag, stl::size_t n) {
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
void vector<T, Allocator>::resize(stl::tags::uninitialized_tag, stl::size_t n) {
    if (_size >= n) { return; }

    reserve(n, stl::tags::uninitialized);
    _size = n;
    // _capacity is set inside reserve()
}

template<typename T, typename Allocator>
void vector<T, Allocator>::push_back(T const& value) {
    // If we have reached the maximum size for our vector
    if (_size == _capacity) {
        grow(calc_grow_size());
    }

    // Now the index _size is guaranteed to be valid
    inplace_construct_n(_data + _size, 1, value);
    _size += 1;
}

template<typename T, typename Allocator>
void vector<T, Allocator>::push_back(T&& value) {
    // If we have reached the maximum size for our vector
    if (_size == _capacity) {
        grow(calc_grow_size());
    }

    // Now the index _size is guaranteed to be valid
    new (_data + _size) T { stl::move(value) };
    _size += 1;
}

template<typename T, typename Allocator>
template<typename... Args>
void vector<T, Allocator>::emplace_back(Args&&... args) {
    if (_size == _capacity) {
        
        grow(calc_grow_size());
    }

    new (_data + _size) T { stl::forward<Args>(args) ... };
    _size += 1;
}

template<typename T, typename Allocator>
void vector<T, Allocator>::clear() {
    destruct_n(_data, _size);
    _size = 0;
}

template<typename T, typename Allocator>
void vector<T, Allocator>::shrink_to_fit() {
    if (_size == _capacity) { return; }
    
    grow(_size);
}

template<typename T, typename Allocator>
typename vector<T, Allocator>::iterator vector<T, Allocator>::insert(iterator pos, T const& value) {
    // Avoid code duplication like a boss
    T cpy = value;
    return insert(stl::move(cpy));
}

template<typename T, typename Allocator>
typename vector<T, Allocator>::iterator vector<T, Allocator>::insert(iterator pos, T&& value) {
    if (pos == end()) { 
        emplace_back(stl::move(value)); 
        return end() - 1; 
    }

    T* ptr = _data;

    if (_size + 1 > _capacity) {
        ptr = allocate(calc_grow_size());
    }

    stl::size_t const index = pos - _data;

    // Shift all elements starting at requested index by one. We do this in reverse order to avoid
    // overwriting values that still have to be moved if no reallocation occurs
    for (stl::size_t i = _size; i > index; --i) {
        ptr[i] = stl::move(_data[i - 1]);
    }

    // Insert the value at requested index
    ptr[index] = stl::move(value);

    // If we had a reallocation, swap the pointers and also move data from before the index
    if (_data != ptr) {
        inplace_move_from_range(ptr, _data, _data + index);

        deallocate(_data, _capacity);
        _data = ptr;
    }
    
    _size += 1;
    return begin() + index;
}

template<typename T, typename Allocator>
typename vector<T, Allocator>::iterator vector<T, Allocator>::erase(iterator pos) {
    STL_ASSERT(pos >= begin() && pos < end(), "invalid iterator given to vector::erase()");

    // Destruct the value at pos
    destruct_n(pos, 1);

    // Move all values behind it back by one
    for (iterator it = pos; it < end() - 1; ++it) {
        *it = stl::move(*(it + 1));
    }
    
    _size -= 1;

    return pos;
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

template<typename T, typename Allocator>
stl::size_t vector<T, Allocator>::calc_grow_size() const {
    return stl::max(1, static_cast<stl::size_t>(_capacity * 2));
}

template<typename T, typename Allocator>
void vector<T, Allocator>::grow(stl::size_t n) {
    // Allocate new array and move elements
    T* new_data = allocate(n);
    inplace_move_from_range(new_data, begin(), end());
    // Deallocate old data
    deallocate(_data, _capacity);
    // Swap
    _capacity = n;
    _data = new_data;
}

} // namespace stl

#endif