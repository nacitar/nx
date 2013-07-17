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
/// Provides a function to  calculate the log10 of an unsigned integer.

#ifndef INCLUDE_NX_LOG10_H_
#define INCLUDE_NX_LOG10_H_

#include "nx/core.h"
#include "nx/constant.h"

namespace nx {
  namespace detail {
    template <unsigned int uVersion, class T>
    constexpr EnableIf<
      All<std::is_unsigned<T>, Bool<uVersion == 64>>,
    unsigned int> log10(const T&v) {
      using constant::pow10_64;
      return  (v < pow10_64[10]) ?
          (v < pow10_64[5]) ?
            (v < pow10_64[2]) ?
              (v < pow10_64[1]) ? 0 : 1
            :
            (v < pow10_64[4]) ?
              (v < pow10_64[3]) ? 2 : 3
            : 4
          :
          (v < pow10_64[7]) ?
            (v < pow10_64[6]) ? 5 : 6
          :
          (v < pow10_64[9]) ?
            (v < pow10_64[8]) ? 7 : 8
          : 9
        :
        (v < pow10_64[15]) ?
          (v < pow10_64[12]) ?
            (v < pow10_64[11]) ? 10 : 11
          :
          (v < pow10_64[14]) ?
            (v < pow10_64[13]) ? 12 : 13
          : 14
        :
        (v < pow10_64[17]) ?
          (v < pow10_64[16]) ? 15 : 16
        :
        (v < pow10_64[19]) ?
          (v < pow10_64[18]) ? 17 : 18
        : 19;
    }
    template <unsigned int uVersion, class T>
    constexpr EnableIf<
      All<std::is_unsigned<T>, Bool<uVersion == 32>>,
    unsigned int> log10(const T&v) {
      using constant::pow10_32;
      return  (v < pow10_32[5]) ?
          (v < pow10_32[2]) ?
            (v < pow10_32[1]) ? 0 : 1
          :
          (v < pow10_32[4]) ?
            (v < pow10_32[3]) ? 2 : 3
          : 4
        :
        (v < pow10_32[7]) ?
          (v < pow10_32[6]) ? 5 : 6
        :
        (v < pow10_32[9]) ?
          (v < pow10_32[8]) ? 7 : 8
        : 9;
    }
  }  // namespace detail

  template <class T>
  inline constexpr EnableIf<
      All<std::is_unsigned<T>, BitRange<T, 0, 32>>,
  unsigned int> log10(const T&v) {
    return detail::log10<32>(v);
  }
  template <class T>
  inline constexpr EnableIf<
      All<std::is_unsigned<T>, BitRange<T, 33, 64>>,
  unsigned int> log10(const T&v) {
    return detail::log10<64>(v);
  }

}  // namespace nx

#endif  // INCLUDE_NX_LOG10_H_
