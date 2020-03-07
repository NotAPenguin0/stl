#ifndef SATURN_STL_ASSERT_HPP_
#define SATURN_STL_ASSERT_HPP_

#include <cassert>

#define STL_ASSERT(cond, msg) assert(cond && msg)

#endif