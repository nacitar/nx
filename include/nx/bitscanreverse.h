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

#ifndef INCLUDE_NX_BITSCANREVERSE_H_
#define INCLUDE_NX_BITSCANREVERSE_H_

#include "nx/core.h"

namespace nx {
  // Enable this define to not use compiler builtins.
  //#define NX_USE_GENERIC_BITSCANREVERSE
  #if !defined(NX_USE_GENERIC_BITSCANREVERSE) && defined(NX_TC_GCC)
  // GCC bitScanReverse

  // unsigned long long version
  template <class T>
  inline constexpr EnableIf<
      All<
        std::is_integral<T>,
        IntegerFits<T, unsigned long long>,
        Not<IntegerFits<T, unsigned long>>
      >,
  unsigned int> bitScanReverse(const T&val) {
    return (val ? BitSize<T>::value - 1 -__builtin_clzll(val) : 0);
  }
  // unsigned long version
  template <class T>
  inline constexpr EnableIf<
      All<
        std::is_integral<T>,
        IntegerFits<T, unsigned long>,
        Not<IntegerFits<T, unsigned int>>
      >,
  unsigned int> bitScanReverse(const T&val) {
    return (val ? BitSize<T>::value - 1 -__builtin_clzl(val) : 0);
  }
  // unsigned int version
  template <class T>
  inline constexpr EnableIf<
      All<
        std::is_integral<T>,
        IntegerFits<T, unsigned int>
      >,
  unsigned int> bitScanReverse(const T&val) {
    return (val ? BitSize<T>::value - 1 -__builtin_clz(val) : 0);
  }
  #else
  // Generic bitScanReverse - finds the highest set bit index

  namespace detail {
    template <const unsigned int uVersion,class T>
    inline constexpr EnableIf<
      All<std::is_integral<T>,Bool<uVersion==8>>,
    unsigned int> bitScanReverse(const T&v) {
      using constant::Log256;
      return (
        // for types <= 8
        Log256[v]
      );
    }
    template <const unsigned int uVersion,class T>
    inline constexpr EnableIf<
      All<std::is_integral<T>,Bool<uVersion==16>>,
    unsigned int> bitScanReverse(const T&v) {
      using constant::Log256;
      return (
        // for types <= 16
        (v >> 8) ? (8 + Log256[v >> 8]) : (
          bitScanReverse<8>(v)
        )
      );
    }
    template <const unsigned int uVersion,class T>
    constexpr EnableIf<
      All<std::is_integral<T>,Bool<uVersion==32>>,
    unsigned int> bitScanReverse(const T&v) {
      using constant::Log256;
      return (
        // for types <= 32
        (v >> 16) ? (
          (v >> 24) ? (24 + Log256[v >> 24]) : (16 + Log256[v >> 16])
        ) : (
          bitScanReverse<16>(v)
        )
      );
    }
    template <const unsigned int uVersion,class T>
    constexpr EnableIf<
      All<std::is_integral<T>,Bool<uVersion==64>>,
    unsigned int> bitScanReverse(const T&v) {
      using constant::Log256;
      return (
        // for types <= 64
        (v >> 32) ? (
          (v >> 48) ? (
            (v >> 56) ? (56 + Log256[v >> 56]) : (48 + Log256[v >> 48])
          ) : (
            (v >> 40) ? (40 + Log256[v >> 40]) : (32 + Log256[v >> 32])
          )
        ) : (
          bitScanReverse<32>(v)
        )
      );
    }
  }
  template <class T>
  inline constexpr EnableIf<
      All<std::is_integral<T>, BitRange<T,0,8>>,
  unsigned int> bitScanReverse(const T&v) {
    return detail::bitScanReverse<8>(v);
  }
  template <class T>
  inline constexpr EnableIf<
      All<std::is_integral<T>, BitRange<T,9,16>>,
  unsigned int> bitScanReverse(const T&v) {
    return detail::bitScanReverse<16>(v);
  }
  template <class T>
  inline constexpr EnableIf<
      All<std::is_integral<T>, BitRange<T,17,32>>,
  unsigned int> bitScanReverse(const T&v) {
    return detail::bitScanReverse<32>(v);
  }
  template <class T>
  inline constexpr EnableIf<
      All<std::is_integral<T>, BitRange<T,33,64>>,
  unsigned int> bitScanReverse(const T&v) {
    return detail::bitScanReverse<64>(v);
  }
  #if 0
  // TODO: when we get static warning working
  template <class T>
  inline constexpr EnableIf<
      All<std::is_integral<T>, BitRange<T,65>>,
  unsigned int> bitScanReverse(const T&v) {
    return detail::bitScanReverse<64>(v & BitMask<T,64>::value);
  }
  #endif
  #endif

}
#endif  // INCLUDE_NX_BITSCANREVERSE_H_
