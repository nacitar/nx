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

/// @file
/// Provides a function to find the bit index of the least significant set bit
/// in an unsigned integral value.

#ifndef INCLUDE_NX_BITSCANFORWARD_H_
#define INCLUDE_NX_BITSCANFORWARD_H_

#include "nx/core.h"

namespace nx {
  // Enable this define to not use compiler builtins.
  //#define NX_USE_GENERIC_BITSCANFORWARD

  #if !defined(NX_USE_GENERIC_BITSCANFORWARD) && defined(NX_TC_GCC)
  // GCC bitScanForward - finds the lowest set bit index

  // unsigned long long version
  template <class T>
  inline constexpr EnableIf<
      All<
        std::is_integral<T>,
        IntegerFits<T, unsigned long long>,
        Not<IntegerFits<T, unsigned long>>
      >,
  unsigned int> bitScanForward(const T&val) {
    return (val ? __builtin_ctzll(val) : 0);
  }
  // unsigned long version
  template <class T>
  inline constexpr EnableIf<
      All<
        std::is_integral<T>,
        IntegerFits<T, unsigned long>,
        Not<IntegerFits<T, unsigned int>>
      >,
  unsigned int> bitScanForward(const T&val) {
    return (val ? __builtin_ctzl(val) : 0);
  }
  // unsigned int version
  template <class T>
  inline constexpr EnableIf<
      All<
        std::is_integral<T>,
        IntegerFits<T, unsigned int>
      >,
  unsigned int> bitScanForward(const T&val) {
    return (val ? __builtin_ctz(val) : 0);
  }
  #else
  // Generic bitScanForward

  namespace detail {
    template <const unsigned int uVersion,class T>
    inline constexpr EnableIf<
      All<std::is_integral<T>,Bool<uVersion==64>>,
    unsigned int> bitScanForward(const T&val) {
      typedef typename std::make_signed<
        typename std::add_const<T>::type
      >::type const_signed_T;
      typedef typename std::make_unsigned<
        typename std::add_const<T>::type
      >::type const_unsigned_T;
      return constant::deBruijn64[
          (
            (
              static_cast<const_unsigned_T>(
                val & -static_cast<const_signed_T&>(val)
              ) * constant::deBruijn64Multiplier
            ) >> 58u
          ) & 0x3Fu
      ];
    }
    template <const unsigned int uVersion,class T>
    inline constexpr EnableIf<
      All<std::is_integral<T>,Bool<uVersion==32>>,
    unsigned int> bitScanForward(const T&val) {
      typedef typename std::make_signed<
        typename std::add_const<T>::type
      >::type const_signed_T;
      typedef typename std::make_unsigned<
        typename std::add_const<T>::type
      >::type const_unsigned_T;
      return constant::deBruijn32[
          (
            (
              static_cast<const_unsigned_T>(
                val & -static_cast<const_signed_T&>(val)
              ) * constant::deBruijn32Multiplier
            ) >> 27u
          ) & 0x1Fu
      ];
    }
  }
  template <class T>
  inline constexpr EnableIf<
      All<std::is_integral<T>, BitRange<T,0,32>>,
  unsigned int> bitScanForward(const T&v) {
    return detail::bitScanForward<32>(v);
  }
  template <class T>
  inline constexpr EnableIf<
      All<std::is_integral<T>, BitRange<T,33,64>>,
  unsigned int> bitScanForward(const T&v) {
    return detail::bitScanForward<64>(v);
  }
  #endif

}
#endif  // INCLUDE_NX_BITSCANFORWARD_H_
