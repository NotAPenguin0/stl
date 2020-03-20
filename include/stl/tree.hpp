#ifndef STL_TREE_HPP_
#define STL_TREE_HPP_

#include <stl/vector.hpp>
#include <list>

#include <stl/tuple.hpp>

namespace stl {

namespace detail {

// Rip cache, might need a better solution
template<typename T>
using children_storage = std::list<T>;

template<typename T>
struct tree_node {
    T data = T{};
    children_storage<tree_node<T>> children = {};
    tree_node<T>* parent = nullptr;
};

} // namespace detail

template<typename T>
class tree {
public:
    using leaf_type = detail::tree_node<T>;

    class iterator {
    public:
        iterator() = default;
        iterator(leaf_type* leaf) noexcept;

        iterator(iterator const&) = default;
        iterator& operator=(iterator const&) = default;

        T& operator*();
        T const& operator*() const;

        leaf_type* operator->();
        leaf_type const* operator->() const;

        bool valid() const noexcept;

        leaf_type* leaf() noexcept;
        leaf_type* parent() noexcept;

    private:
        leaf_type* _leaf = nullptr;
        leaf_type* _parent = nullptr;
    };

    class const_iterator {
    public:
        const_iterator() = default;
        const_iterator(iterator it) noexcept;
        const_iterator(leaf_type const* leaf) noexcept;

        const_iterator(const_iterator const&) = default;
        const_iterator& operator=(const_iterator const&) = default;

        T const& operator*() const;

        leaf_type const* operator->() const;

        bool valid() const noexcept;

        leaf_type const* leaf() const noexcept;
        leaf_type const* parent() const noexcept;

    private:
        leaf_type const* _leaf = nullptr;
        leaf_type const* _parent = nullptr;
    };

    iterator root();
    const_iterator root() const;
    
    struct traverse_info {
        stl::size_t level = 0;
        iterator it;
        iterator parent;
    };

    struct const_traverse_info {
        stl::size_t level = 0;
        const_iterator it;
        const_iterator parent;
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

    template<typename F, typename PostF, typename Arg, typename... Args>
    void traverse(F&& f, PostF&& post_callback, Arg&& arg, Args&&... args);

    template<typename F, typename PostF, typename Arg, typename... Args>
    void traverse(F&& f, PostF&& post_callback, Arg&& arg, Args&&... args) const;

    template<typename F, typename Arg, typename... Args>
    void traverse_from(iterator it, F&& f, Arg&& arg, Args&&... args);

    template<typename F, typename Arg, typename... Args>
    void traverse_from(const_iterator it, F&& f, Arg&& arg, Args&&... args) const;

    iterator insert(iterator parent, T const& value);
    iterator insert(iterator parent, T&& value);

    iterator find(T const& value);
    const_iterator find(T const& value) const;

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
    
    template<typename F, typename PostF, typename Arg, typename... Args>
    void traverse_impl(F&& f, PostF&& post_callback, leaf_type* leaf, stl::size_t level, Arg&& arg, Args&&... args);

    template<typename F, typename PostF, typename Arg, typename... Args>
    void traverse_impl(F&& f, PostF&& post_callback, leaf_type const* leaf, stl::size_t level, Arg&& arg, Args&&... args) const;
};

template<typename T>
tree<T>::iterator::iterator(leaf_type* leaf) : _leaf(leaf) {
    if (leaf && leaf->parent) {
        _parent = leaf->parent;
    }
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
auto tree<T>::iterator::operator->() -> leaf_type* {
    return _leaf;
}

template<typename T>
auto tree<T>::iterator::operator->() const -> leaf_type const* {
    return _leaf;
}

template<typename T>
bool tree<T>::iterator::valid() const {
    return _leaf != nullptr;
}

template<typename T>
auto tree<T>::iterator::leaf() -> leaf_type* {
    return _leaf;
}

template<typename T>
auto tree<T>::iterator::parent() -> leaf_type* {
    return _parent;
}

template<typename T>
tree<T>::const_iterator::const_iterator(leaf_type const* leaf) : _leaf(leaf) {
    if (leaf && leaf->parent) {
        _parent = leaf->parent;
    }
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
auto tree<T>::const_iterator::operator->() const -> leaf_type const*  {
    return _leaf;
}

template<typename T>
bool tree<T>::const_iterator::valid() const {
    return _leaf != nullptr;
}

template<typename T>
auto tree<T>::const_iterator::leaf() const -> leaf_type const* {
    return _leaf;
}

template<typename T>
auto tree<T>::const_iterator::parent() const -> leaf_type const* {
    return _parent;
}


template<typename T>
auto tree<T>::root() -> iterator {
    return iterator(&_root);
}

template<typename T>
auto tree<T>::root() const -> const_iterator {
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
    traverse_info info{ level, iterator(leaf), iterator(leaf).parent() };
    f(leaf->data, info);
    for (auto& child : leaf->children) {
        traverse_impl(f, &child, level + 1);
    }
}

template<typename T>
template<typename F>
void tree<T>::traverse_impl(F&& f, leaf_type const* leaf, stl::size_t level) const {
    const_traverse_info info{ level, const_iterator(leaf), const_iterator(leaf).parent() };
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

// Post callback
template<typename T>
template<typename F, typename PostF, typename Arg, typename... Args>
void tree<T>::traverse(F&& f, PostF&& post_callback, Arg&& arg, Args&&... args) {
    traverse_impl(stl::forward<F>(f), stl::forward<PostF>(post_callback), 
        &_root, 0, stl::forward<Arg>(arg), stl::forward<Args>(args) ...);
}

template<typename T>
template<typename F, typename PostF, typename Arg, typename... Args>
void tree<T>::traverse(F&& f, PostF&& post_callback, Arg&& arg, Args&&... args) const {
    traverse_impl(stl::forward<F>(f), stl::forward<PostF>(post_callback),
        &_root, 0, stl::forward<Arg>(arg), stl::forward<Args>(args) ...);
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
auto apply_tuple_impl(Tuple& t, F&& f, Args&&... args, stl::index_sequence<Is...>) {
    return f(stl::forward<Args>(args) ..., stl::get<Is>(t) ...);
}

template<typename Tuple, typename F, typename... Args>
auto apply_tuple(Tuple& t, F&& f, Args&&... args) {
    return apply_tuple_impl(t, stl::forward<F>(f), stl::forward<Args>(args) ..., stl::make_index_sequence<stl::tuple_size_v<Tuple>>{});
}

template<typename Tuple, typename F, typename... Args, stl::size_t... Is>
auto apply_tuple_impl(Tuple const& t, F&& f, Args&&... args, stl::index_sequence<Is...>) {
    return f(stl::forward<Args>(args) ..., stl::get<Is>(t) ...);
}

template<typename Tuple, typename F, typename... Args>
auto apply_tuple(Tuple const& t, F&& f, Args&&... args) {
    return apply_tuple_impl(t, stl::forward<F>(f), stl::forward<Args>(args) ..., stl::make_index_sequence<stl::tuple_size_v<Tuple>>{});
}

} // namespace detail

template<typename T>
template<typename F, typename Arg, typename... Args>
void tree<T>::traverse_impl(F&& f, leaf_type* leaf, stl::size_t level, Arg&& arg, Args&&... args) {
    traverse_info info { level, iterator(leaf), iterator(leaf).parent() };
    auto child_call_args = f(leaf->data, info, stl::forward<Arg>(arg), stl::forward<Args>(args) ...);
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
    const_traverse_info info{ level, const_iterator(leaf), const_iterator(leaf).parent() };
    auto child_call_args = f(leaf->data, info, stl::forward<Arg>(arg), stl::forward<Args>(args) ...);
    for (auto const& child : leaf->children) {
        auto call_traverse_impl = [this, &f, &child, level](auto&&... child_call_args) {
            traverse_impl(stl::forward<F>(f), &child, level + 1, child_call_args ...);
        };
        detail::apply_tuple(child_call_args, call_traverse_impl);
    }
}

template<typename T>
template<typename F, typename PostF, typename Arg, typename... Args>
void tree<T>::traverse_impl(F&& f, PostF&& post_callback, leaf_type* leaf, stl::size_t level, Arg&& arg, Args&&... args) {
    traverse_info info { level, iterator(leaf), iterator(leaf).parent() };
    auto child_call_args = f(leaf->data, info, stl::forward<Arg>(arg), stl::forward<Args>(args) ...);
    for (auto& child : leaf->children) {
        auto call_traverse_impl = [this, &f, &post_callback, &child, level](auto&&... child_call_args) {
            traverse_impl(stl::forward<F>(f), stl::forward<PostF>(post_callback), &child, level + 1, child_call_args ...);
        };
        detail::apply_tuple(child_call_args, call_traverse_impl);
    }
    // After a node has been processed, call the post callback
    post_callback(leaf->data, info, stl::forward<Arg>(arg), stl::forward<Args>(args) ...);
}

template<typename T>
template<typename F, typename PostF, typename Arg, typename... Args>
void tree<T>::traverse_impl(F&& f, PostF&& post_callback, leaf_type const* leaf, stl::size_t level, Arg&& arg, Args&&... args) const {
    const_traverse_info info{ level, const_iterator(leaf), const_iterator(leaf).parent() };
    auto child_call_args = f(leaf->data, info, stl::forward<Arg>(arg), stl::forward<Args>(args) ...);
    for (auto const& child : leaf->children) {
        auto call_traverse_impl = [this, &f, &post_callback, &child, level](auto&&... child_call_args) {
            traverse_impl(stl::forward<F>(f), stl::forward<PostF>(post_callback), &child, level + 1, child_call_args ...);
        };
        detail::apply_tuple(child_call_args, call_traverse_impl);
    }
    post_callback(leaf->data, info, stl::forward<Arg>(arg), stl::forward<Args>(args) ...);
}

template<typename T>
auto tree<T>::insert(iterator parent, T const& value) -> iterator {
    T v = value;
    return insert(parent, stl::move(v));
}

template<typename T>
auto tree<T>::insert(iterator parent, T&& value) -> iterator {
    parent->children.push_back(leaf_type{ stl::move(value), detail::children_storage<leaf_type>{}, parent.leaf() });
    return iterator(&parent->children.back());
}

template<typename T>
auto tree<T>::find(T const& value) const -> const_iterator {
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
auto tree<T>::find(T const& value) -> iterator {
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