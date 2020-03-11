#ifndef SATURN_STL_UNIQUE_PTR_HPP_
#define SATURN_STL_UNIQUE_PTR_HPP_

#include <stl/utility.hpp>
#include <stl/assert.hpp>

namespace stl {

template<typename T>
class unique_ptr {
public:
    constexpr unique_ptr() noexcept = default;
    constexpr unique_ptr(std::nullptr_t) noexcept;

    explicit unique_ptr(T* ptr);

    unique_ptr(unique_ptr const&) = delete;
    unique_ptr(unique_ptr&& rhs);

    template<typename U>
    unique_ptr(unique_ptr<U>&& rhs);

    unique_ptr& operator=(unique_ptr const&) = delete;
    unique_ptr& operator=(unique_ptr&& rhs);

    template<typename U>
    unique_ptr& operator=(unique_ptr<U>&& rhs);

    ~unique_ptr();

    T* release();
    void reset(T* new_ptr = nullptr);

    T* get();
    T const* get() const;

    explicit operator bool() const;

    T& operator*();
    T const& operator*() const;

    T* operator->();
    T const* operator->() const;

private:
    T* _ptr = nullptr;

    void do_delete();
};

template<typename T>
constexpr unique_ptr<T>::unique_ptr(std::nullptr_t) noexcept : _ptr(nullptr) {

}

template<typename T>
unique_ptr<T>::unique_ptr(T* ptr) : _ptr(ptr) {

}


template<typename T>
unique_ptr<T>::unique_ptr(unique_ptr&& rhs) : _ptr(rhs._ptr) {
    rhs._ptr = nullptr;
}

template<typename T>
template<typename U>
unique_ptr<T>::unique_ptr(unique_ptr<U>&& rhs) : _ptr(rhs._ptr) {
    rhs._ptr = nullptr;
}

template<typename T>
unique_ptr<T>& unique_ptr<T>::operator=(unique_ptr&& rhs) {
    if (this != &rhs) {
        do_delete();
        _ptr = rhs._ptr;
        rhs._ptr = nullptr;
    }
    return *this;
}

template<typename T>
template<typename U>
unique_ptr<T>& unique_ptr<T>::operator=(unique_ptr<U>&& rhs) {
    do_delete();
    _ptr = rhs._ptr;
    rhs._ptr = nullptr;
    return *this;
}

template<typename T>
unique_ptr<T>::~unique_ptr() {
    do_delete();
}

template<typename T>
T* unique_ptr<T>::release() {
    T* stored = _ptr;
    _ptr = nullptr;
    return stored;
}

template<typename T>
void unique_ptr<T>::reset(T* new_ptr) {
    do_delete();
    _ptr = new_ptr;
}

template<typename T>
T* unique_ptr<T>::get() {
    return _ptr;
}

template<typename T>
T const* unique_ptr<T>::get() const {
    return _ptr;
}

template<typename T>
unique_ptr<T>::operator bool() const {
    return _ptr != nullptr;
}

template<typename T>
T& unique_ptr<T>::operator*() {
    STL_ASSERT(_ptr, "Cannot dereference null unique_ptr");
    return *_ptr;
}

template<typename T>
T const& unique_ptr<T>::operator*() const {
    STL_ASSERT(_ptr, "Cannot dereference null unique_ptr");
    return *_ptr;
}

template<typename T>
T* unique_ptr<T>::operator->() {
    STL_ASSERT(_ptr, "Cannot dereference null unique_ptr");
    return _ptr;
}

template<typename T>
T const* unique_ptr<T>::operator->() const {
    STL_ASSERT(_ptr, "Cannot dereference null unique_ptr");
    return _ptr;
}

template<typename T>
void unique_ptr<T>::do_delete() {
    delete _ptr;
    _ptr = nullptr;
}

template<typename T, typename... Args>
unique_ptr<T> make_unique(Args&&... args) {
    return unique_ptr<T>(new T (stl::forward<Args>(args) ... ));
}

}

#endif