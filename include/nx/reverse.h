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
/// Provides a function to reverse the bits of an integral value.

#ifndef INCLUDE_NX_REVERSE_H_
#define INCLUDE_NX_REVERSE_H_

#include "nx/core.h"

namespace nx {

  namespace detail {
    template <unsigned int uVersion, class T>
    constexpr EnableIf<
      All<std::is_unsigned<T>, Bool<uVersion == 64>>,
    T> Reverse(T v) {
      using constant::Reverse256;
      return
          static_cast<T>(Reverse256[ v        & 0xff]) << 56 |
          static_cast<T>(Reverse256[(v >>  8) & 0xff]) << 48 |
          static_cast<T>(Reverse256[(v >> 16) & 0xff]) << 40 |
          static_cast<T>(Reverse256[(v >> 24) & 0xff]) << 32 |
          static_cast<T>(Reverse256[(v >> 32) & 0xff]) << 24 |
          static_cast<T>(Reverse256[(v >> 40) & 0xff]) << 16 |
          static_cast<T>(Reverse256[(v >> 48) & 0xff]) <<  8 |
          static_cast<T>(Reverse256[(v >> 56) & 0xff]);
    }

    template <unsigned int uVersion, class T>
    constexpr EnableIf<
      All<std::is_unsigned<T>, Bool<uVersion == 32>>,
    T> Reverse(T v) {
      using constant::Reverse256;
      return
          static_cast<T>(Reverse256[ v        & 0xff]) << 24 |
          static_cast<T>(Reverse256[(v >>  8) & 0xff]) << 16 |
          static_cast<T>(Reverse256[(v >> 16) & 0xff]) <<  8 |
          static_cast<T>(Reverse256[(v >> 24) & 0xff]);
    }

    template <unsigned int uVersion, class T>
    constexpr EnableIf<
      All<std::is_unsigned<T>, Bool<uVersion == 16>>,
    T> Reverse(T v) {
      using constant::Reverse256;
      return
          static_cast<T>(Reverse256[ v        & 0xff]) <<  8 |
          static_cast<T>(Reverse256[(v >>  8) & 0xff]);
    }

    template <unsigned int uVersion, class T>
    constexpr EnableIf<
      All<std::is_unsigned<T>, Bool<uVersion == 8>>,
    T> Reverse(T v) {
      using constant::Reverse256;
      return
          static_cast<T>(Reverse256[v]);
    }
  }  // namespace detail

  template <class T>
  inline constexpr EnableIf<
      All<std::is_unsigned<T>, BitRange<T, 0, 8>>,
  unsigned int> Reverse(T v) {
    return detail::Reverse<8>(v);
  }

  template <class T>
  inline constexpr EnableIf<
      All<std::is_unsigned<T>, BitRange<T, 9, 16>>,
  unsigned int> Reverse(T v) {
    return detail::Reverse<16>(v);
  }

  template <class T>
  inline constexpr EnableIf<
      All<std::is_unsigned<T>, BitRange<T, 17, 32>>,
  unsigned int> Reverse(T v) {
    return detail::Reverse<32>(v);
  }

  template <class T>
  inline constexpr EnableIf<
      All<std::is_unsigned<T>, BitRange<T, 33, 64>>,
  unsigned int> Reverse(T v) {
    return detail::Reverse<64>(v);
  }

  template <class T>
  inline constexpr EnableIf<
      std::is_signed<T>,
  unsigned int> Reverse(T v) {
    typedef typename std::make_unsigned<T>::type UT;
    return Reverse(static_cast<UT>(v));
  }
}  // namespace nx

#endif  // INCLUDE_NX_REVERSE_H_
