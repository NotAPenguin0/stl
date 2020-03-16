#ifndef STL_TREE_HPP_
#define STL_TREE_HPP_

#include <stl/vector.hpp>

#include <tuple>

namespace stl {

namespace detail {

template<typename T>
struct tree_node {
    T data = T{};
    stl::vector<tree_node<T>> children = {};
};

} // namespace detail

template<typename T>
class tree {
public:
    using leaf_type = detail::tree_node<T>;

    class iterator {
    public:
        iterator() = default;
        iterator(leaf_type* leaf);

        iterator(iterator const&) = default;
        iterator& operator=(iterator const&) = default;

        T& operator*();
        T const& operator*() const;

        leaf_type* operator->();
        leaf_type const* operator->() const;

        bool valid() const;

        leaf_type* leaf();

    private:
        leaf_type* _leaf = nullptr;
    };

    class const_iterator {
    public:
        const_iterator() = default;
        const_iterator(iterator it);
        const_iterator(leaf_type const* leaf);

        const_iterator(const_iterator const&) = default;
        const_iterator& operator=(const_iterator const&) = default;

        T const& operator*() const;

        leaf_type const* operator->() const;

        bool valid() const;

        leaf_type const* leaf() const;

    private:
        leaf_type const* _leaf = nullptr;
    };

    iterator root();
    const_iterator root() const;
    
    struct traverse_info {
        stl::size_t level = 0;
        iterator it;
    };

    struct const_traverse_info {
        stl::size_t level = 0;
        const_iterator it;
    };

    template<typename F>
    void traverse(F&& f);

    template<typename F>
    void traverse(F&& f) const;

    template<typename F>
    void traverse_from(iterator it, F&& f);

    template<typename F>
    void traverse_from(const_iterator it, F&& f) const;

    template<typename F, typename Arg, typename... Args>
    void traverse(F&& f, Arg&& arg, Args&&... args);

    template<typename F, typename Arg, typename... Args>
    void traverse(F&& f, Arg&& arg, Args&&... args) const;

    template<typename F, typename Arg, typename... Args>
    void traverse_from(iterator it, F&& f, Arg&& arg, Args&&... args);

    template<typename F, typename Arg, typename... Args>
    void traverse_from(const_iterator it, F&& f, Arg&& arg, Args&&... args) const;

    iterator insert(iterator parent, T const& value);
    iterator insert(iterator parent, T&& value);

    const_iterator find(T const& value) const;
    iterator find(T const& value);

private:
    leaf_type _root;

    template<typename F>
    void traverse_impl(F&& f, leaf_type* leaf, stl::size_t level);

    template<typename F>
    void traverse_impl(F&& f, leaf_type const* leaf, stl::size_t level) const;

    template<typename F, typename Arg, typename... Args>
    void traverse_impl(F&& f, leaf_type* leaf, stl::size_t level, Arg&& arg, Args&&... args);

    template<typename F, typename Arg, typename... Args>
    void traverse_impl(F&& f, leaf_type const* leaf, stl::size_t level, Arg&& arg, Args&&... args) const;
};

template<typename T>
tree<T>::iterator::iterator(leaf_type* leaf) : _leaf(leaf) {

}

template<typename T>
T& tree<T>::iterator::operator*() {
    return _leaf->data;
}

template<typename T>
T const& tree<T>::iterator::operator*() const {
    return _leaf->data;
}

template<typename T>
typename tree<T>::leaf_type* tree<T>::iterator::operator->() {
    return _leaf;
}

template<typename T>
typename tree<T>::leaf_type const* tree<T>::iterator::operator->() const {
    return _leaf;
}

template<typename T>
bool tree<T>::iterator::valid() const {
    return _leaf != nullptr;
}

template<typename T>
typename tree<T>::leaf_type* tree<T>::iterator::leaf() {
    return _leaf;
}

template<typename T>
tree<T>::const_iterator::const_iterator(leaf_type const* leaf) : _leaf(leaf) {

}

template<typename T>
tree<T>::const_iterator::const_iterator(iterator it) {
    if (!it.valid()) { _leaf = nullptr; }
    else {
        _leaf = it.leaf();
    }
}

template<typename T>
T const& tree<T>::const_iterator::operator*() const {
    return _leaf->data;
}

template<typename T>
typename tree<T>::leaf_type const* tree<T>::const_iterator::operator->() const {
    return _leaf;
}

template<typename T>
bool tree<T>::const_iterator::valid() const {
    return _leaf != nullptr;
}

template<typename T>
typename tree<T>::leaf_type const* tree<T>::const_iterator::leaf() const {
    return _leaf;
}


template<typename T>
typename tree<T>::iterator tree<T>::root() {
    return iterator(&_root);
}

template<typename T>
typename tree<T>::const_iterator tree<T>::root() const {
    return const_iterator(&_root);
}

// NO-ARGUMENT TRAVERSE

template<typename T>
template<typename F>
void tree<T>::traverse(F&& f) {
    traverse_impl(stl::forward<F>(f), &_root, 0);
}

template<typename T>
template<typename F>
void tree<T>::traverse(F&& f) const {
    traverse_impl(stl::forward<F>(f), &_root, 0);
}

template<typename T>
template<typename F>
void tree<T>::traverse_from(iterator it, F&& f) {
    traverse_impl(stl::forward<F>(f), it.leaf(), 0);
}

template<typename T>
template<typename F>
void tree<T>::traverse_from(const_iterator it, F&& f) const {
    traverse_impl(stl::forward<F>(f), it.leaf(), 0);
}

template<typename T>
template<typename F>
void tree<T>::traverse_impl(F&& f, leaf_type* leaf, stl::size_t level) {
    traverse_info info{ level, iterator(leaf) };
    f(leaf->data, info);
    for (auto& child : leaf->children) {
        traverse_impl(f, &child, level + 1);
    }
}

template<typename T>
template<typename F>
void tree<T>::traverse_impl(F&& f, leaf_type const* leaf, stl::size_t level) const {
    const_traverse_info info{ level, const_iterator(leaf) };
    f(leaf->data, info);
    for (auto const& child : leaf->children) {
        traverse_impl(f, &child, level + 1);
    }
}

// ARGUMENT TRAVERSE

template<typename T>
template<typename F, typename Arg, typename... Args>
void tree<T>::traverse(F&& f, Arg&& arg, Args&&... args) {
    traverse_impl(stl::forward<F>(f), &_root, 0, stl::forward<Arg>(arg), stl::forward<Args>(args) ...);
}

template<typename T>
template<typename F, typename Arg, typename... Args>
void tree<T>::traverse(F&& f, Arg&& arg, Args&&... args) const {
    traverse_impl(stl::forward<F>(f), &_root, 0, stl::forward<Arg>(arg), stl::forward<Args>(args) ...);
}

template<typename T>
template<typename F, typename Arg, typename... Args>
void tree<T>::traverse_from(iterator it, F&& f, Arg&& arg, Args&&... args) {
    traverse_impl(stl::forward<F>(f), it.leaf(), 0, stl::forward<Arg>(arg), stl::forward<Args>(args) ...);
}

template<typename T>
template<typename F, typename Arg, typename... Args>
void tree<T>::traverse_from(const_iterator it, F&& f, Arg&& arg, Args&&... args) const {
    traverse_impl(stl::forward<F>(f), it.leaf(), 0, stl::forward<Arg>(arg), stl::forward<Args>(args) ...);
}

namespace detail {

template<typename Tuple, typename F, typename... Args, stl::size_t... Is>
auto apply_tuple_impl(Tuple& t, F&& f, Args&&... args, std::index_sequence<Is...>) {
    return f(stl::forward<Args>(args) ..., std::get<Is>(t) ...);
}

template<typename Tuple, typename F, typename... Args>
auto apply_tuple(Tuple& t, F&& f, Args&&... args) {
    return apply_tuple_impl(t, stl::forward<F>(f), stl::forward<Args>(args) ..., std::make_index_sequence<std::tuple_size_v<Tuple>>{});
}

template<typename Tuple, typename F, typename... Args, stl::size_t... Is>
auto apply_tuple_impl(Tuple const& t, F&& f, Args&&... args, std::index_sequence<Is...>) {
    return f(stl::forward<Args>(args) ..., std::get<Is>(t) ...);
}

template<typename Tuple, typename F, typename... Args>
auto apply_tuple(Tuple const& t, F&& f, Args&&... args) {
    return apply_tuple_impl(t, stl::forward<F>(f), stl::forward<Args>(args) ..., std::make_index_sequence<std::tuple_size_v<Tuple>>{});
}

}

template<typename T>
template<typename F, typename Arg, typename... Args>
void tree<T>::traverse_impl(F&& f, leaf_type* leaf, stl::size_t level, Arg&& arg, Args&&... args) {
    traverse_info info { level, iterator(leaf) };
    std::tuple<Arg, Args...> child_call_args = f(leaf->data, info, stl::forward<Arg>(arg), stl::forward<Args>(args) ...);
    for (auto& child : leaf->children) {
        auto call_traverse_impl = [this, &f, &child, level](auto&&... child_call_args) {
            traverse_impl(stl::forward<F>(f), &child, level + 1, child_call_args ...);
        };
        detail::apply_tuple(child_call_args, call_traverse_impl);
    }
}

template<typename T>
template<typename F, typename Arg, typename... Args>
void tree<T>::traverse_impl(F&& f, leaf_type const* leaf, stl::size_t level, Arg&& arg, Args&&... args) const {
    const_traverse_info info{ level, const_iterator(leaf) };
    std::tuple<Arg, Args...> child_call_args = f(leaf->data, info, stl::forward<Arg>(arg), stl::forward<Args>(args) ...);
    for (auto& child : leaf->children) {
        auto call_traverse_impl = [this, &f, &child, level](auto&&... child_call_args) {
            traverse_impl(stl::forward<F>(f), &child, level + 1, child_call_args ...);
        };
        detail::apply_tuple(child_call_args, call_traverse_impl);
    }
}

template<typename T>
typename tree<T>::iterator tree<T>::insert(iterator parent, T const& value) {
    T v = value;
    return insert(parent, stl::move(v));
}

template<typename T>
typename tree<T>::iterator tree<T>::insert(iterator parent, T&& value) {
    parent->children.emplace_back(stl::move(value), stl::vector<leaf_type>{});
    return iterator(&parent->children.back());
}

template<typename T>
typename tree<T>::const_iterator tree<T>::find(T const& value) const {
    struct tree_find_impl {
        void operator()(T const& v, const_traverse_info const& info) {
            if (v == to_find) {
                found = info.it;
            }
        }

        T const& to_find;
        const_iterator found;
    };
    tree_find_impl find_func { value, iterator(nullptr) };
    traverse(find_func);
    return find_func.found;
}

template<typename T>
typename tree<T>::iterator tree<T>::find(T const& value) {
    struct tree_find_impl {
        void operator()(T const& v, traverse_info const& info) {
            if (v == to_find) {
                found = info.it;
            }
        }

        T const& to_find;
        iterator found;
    };
    tree_find_impl find_func { value, iterator(nullptr) };
    traverse(find_func);
    return find_func.found;
}


} // namespace stl

#endif