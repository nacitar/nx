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
/// Useful constants.

#ifndef INCLUDE_NX_CONSTANT_H_
#define INCLUDE_NX_CONSTANT_H_

#include "nx/core.h"

namespace nx {
  namespace constant {
    constexpr const uint_least8_t Log256[256] = {
      0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
      4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
      5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
      5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7
    };

    constexpr const uint_least8_t deBruijn32[32] = {
       0,  1, 28,  2, 29, 14, 24,  3,
      30, 22, 20, 15, 25, 17,  4,  8,
      31, 27, 13, 23, 21, 19, 16,  7,
      26, 12, 18,  6, 11,  5, 10,  9
    };
    constexpr const uint_least32_t deBruijn32Multiplier = 0x077CB531ul;

    constexpr const uint_least8_t deBruijn64[64] = {
       0,  1,  2, 53,  3,  7, 54, 27,
       4, 38, 41,  8, 34, 55, 48, 28,
      62,  5, 39, 46, 44, 42, 22,  9,
      24, 35, 59, 56, 49, 18, 29, 11,
      63, 52,  6, 26, 37, 40, 33, 47,
      61, 45, 43, 21, 23, 58, 17, 10,
      51, 25, 36, 32, 60, 20, 57, 16,
      50, 31, 19, 15, 30, 14, 13, 12
    };
    constexpr const uint_least64_t deBruijn64Multiplier = (
        (static_cast<uint_least64_t>(0x022FDD63ul) << 32) | 0xCC95386Dul);

    constexpr const uint_least64_t pow10_64[20] = {
                         1ull,
                        10ull,
                       100ull,
                      1000ull,
                     10000ull,
                    100000ull,
                   1000000ull,
                  10000000ull,
                 100000000ull,
                1000000000ull,
               10000000000ull,
              100000000000ull,
             1000000000000ull,
            10000000000000ull,
           100000000000000ull,
          1000000000000000ull,
         10000000000000000ull,
        100000000000000000ull,
       1000000000000000000ull,
      10000000000000000000ull
    };

    constexpr const uint_least32_t pow10_32[10] = {
               1ul,
              10ul,
             100ul,
            1000ul,
           10000ul,
          100000ul,
         1000000ul,
        10000000ul,
       100000000ul,
      1000000000ul
    };
  }  // namespace constant
}  // namespace nx

#endif  // INCLUDE_NX_CONSTANT_H_
