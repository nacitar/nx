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
/// Provides a function to calculate the popcount of an integral value.

#ifndef INCLUDE_NX_POPCOUNT_H_
#define INCLUDE_NX_POPCOUNT_H_

#include "nx/core.h"

namespace nx {

  // Enable this define to not use compiler builtins.
  // #define NX_USE_GENERIC_POPCOUNT

  #if !defined(NX_USE_GENERIC_POPCOUNT) && defined(NX_TC_GCC)
  // GCC PopCount - counts the number of set bits 

  // unsigned long long version
  template <class T>
  inline constexpr EnableIf<
      All<
        std::is_integral<T>,
        IntegerFits<T, unsigned long long>,  // NOLINT(runtime/int)
        Not<IntegerFits<T, unsigned long>>  // NOLINT(runtime/int)
      >,
  unsigned int> PopCount(T val) {
    return __builtin_popcountll(val);
  }
  // unsigned long version
  template <class T>
  inline constexpr EnableIf<
      All<
        std::is_integral<T>,
        IntegerFits<T, unsigned long>,  // NOLINT(runtime/int)
        Not<IntegerFits<T, unsigned int>>
      >,
  unsigned int> PopCount(T val) {
    return __builtin_popcountl(val);
  }
  // unsigned int version
  template <class T>
  inline constexpr EnableIf<
      All<
        std::is_integral<T>,
        IntegerFits<T, unsigned int>
      >,
  unsigned int> PopCount(T val) {
    return __builtin_popcount(val);
  }
  #else
  namespace detail {
    // Generic PopCount
    template <unsigned int uVersion, class T>
    constexpr EnableIf<
      All<std::is_unsigned<T>, Bool<uVersion == 64>>,
    unsigned int> PopCount(T v) {
      using nx::constant::PopCount256;
      return static_cast<unsigned int>(
        PopCount256[ v        & 0xff]) +
        PopCount256[(v >> 8 ) & 0xff]  +
        PopCount256[(v >> 16) & 0xff]  +
        PopCount256[(v >> 24) & 0xff]  +
        PopCount256[(v >> 32) & 0xff]  +
        PopCount256[(v >> 40) & 0xff]  +
        PopCount256[(v >> 48) & 0xff]  +
        PopCount256[(v >> 56) & 0xff];
    }

    template <unsigned int uVersion, class T>
    constexpr EnableIf<
      All<std::is_unsigned<T>, Bool<uVersion == 32>>,
    unsigned int> PopCount(T v) {
      using nx::constant::PopCount256;
      return static_cast<unsigned int>(
        PopCount256[ v        & 0xff]) +
        PopCount256[(v >> 8 ) & 0xff]  +
        PopCount256[(v >> 16) & 0xff]  +
        PopCount256[(v >> 24) & 0xff];
    }

    template <unsigned int uVersion, class T>
    constexpr EnableIf<
      All<std::is_unsigned<T>, Bool<uVersion == 16>>,
    unsigned int> PopCount(T v) {
      using nx::constant::PopCount256;
      return static_cast<unsigned int>(
        PopCount256[ v        & 0xff]) +
        PopCount256[(v >> 8 ) & 0xff];
    }

    template <unsigned int uVersion, class T>
    constexpr EnableIf<
      All<std::is_unsigned<T>, Bool<uVersion == 8>>,
    unsigned int> PopCount(T v) {
      using nx::constant::PopCount256;
      return static_cast<unsigned int>(PopCount256[v]);
    }
  }  // namespace detail

  template <class T>
  inline constexpr EnableIf<
      All<std::is_unsigned<T>, BitRange<T, 0, 8>>,
  unsigned int> PopCount(T v) {
    return detail::PopCount<8>(v);
  }

  template <class T>
  inline constexpr EnableIf<
      All<std::is_unsigned<T>, BitRange<T, 9, 16>>,
  unsigned int> PopCount(T v) {
    return detail::PopCount<16>(v);
  }

  template <class T>
  inline constexpr EnableIf<
      All<std::is_unsigned<T>, BitRange<T, 17, 32>>,
  unsigned int> PopCount(T v) {
    return detail::PopCount<32>(v);
  }

  template <class T>
  inline constexpr EnableIf<
      All<std::is_unsigned<T>, BitRange<T, 33, 64>>,
  unsigned int> PopCount(T v) {
    return detail::PopCount<64>(v);
  }

  template <class T>
  inline constexpr EnableIf<
      std::is_signed<T>,
  unsigned int> PopCount(T v) {
    typedef typename std::make_unsigned<T>::type UT;
    return PopCount(static_cast<UT>(v));
  }
  #endif
}  // namespace nx

#endif  // INCLUDE_NX_POPCOUNT_H_
