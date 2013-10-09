//
// Copyright (C) 2013 Jacob McIntosh <nacitar at ubercpp dot com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

/// @file literal.h
/// @brief Provides various user-defined literals.

#ifndef INCLUDE_NX_LITERAL_H_
#define INCLUDE_NX_LITERAL_H_

#include "nx/core.h"
#include "nx/digits.h"

/// @brief Library namespace.
namespace nx {

/// @cond nx_detail
namespace detail {

/// @brief definition of binary literal conversion helper.
template<class Enable, char... digits>
struct binary_literal_helper;
/// @brief version for a _valid_ non-final digit.
template<char high, char... digits>
struct binary_literal_helper<
    EnableIf<Bool<ValidDigit<2>(high)>>,
    high,
    digits...> {
  // +1 to include 'high'
  typedef uint_least_t<sizeof...(digits)+1> uint_type;
  static uint_type const value =
    (static_cast<uint_type>(high - '0') << sizeof...(digits))
    + binary_literal_helper<void, digits...>::value;
};

/// @brief version for a _valid_ final digit.
template<char high>
struct binary_literal_helper<
    EnableIf<Bool<ValidDigit<2>(high)>>,
    high> {
  typedef uint_least_t<1> uint_type;
  static uint_type const value = (high - '0');
};

/// @brief dummy implementation for invalid digits to assert.
template<char high, char... digits>
struct binary_literal_helper<
    EnableIf<Not<Bool<ValidDigit<2>(high)>>>,
    high,
    digits...> {
  typedef uint_least_t<sizeof...(digits)+1> uint_type;
  static_assert(ValidDigit<2>(high), "invalid binary digit!");
  static uint_type const value = 0;
};

/// @brief dummy implementation for an invalid final digit to assert.
template<char high>
struct binary_literal_helper<
    EnableIf<Not<Bool<ValidDigit<2>(high)>>>,
    high> {
  typedef uint_least_t<1> uint_type;
  static_assert(ValidDigit<2>(high), "invalid binary digit!");
  static uint_type const value = 0;
};

}  // namespace detail
/// @endcond

}  // namespace nx

/// @brief A binary literal, implemented as a user-defined literal.  This
/// allows specification of binary literals via: @code 10110110_nx_b @endcode
template<char... digits>
constexpr nx::uint_least_t<sizeof...(digits)> operator "" _nx_b() {
  return nx::detail::binary_literal_helper<void, digits...>::value;
}

#endif  // INCLUDE_NX_LITERAL_H_
