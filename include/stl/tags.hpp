#ifndef STL_TAGS_HPP_
#define STL_TAGS_HPP_

namespace stl::tags {

struct reserve_tag {};
constexpr inline reserve_tag reserve;

struct uninitialized_tag {};
constexpr inline uninitialized_tag uninitialized;

struct range_construct_tag {};
constexpr inline range_construct_tag range_construct;

}

#endif