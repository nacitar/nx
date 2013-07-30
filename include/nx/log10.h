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

/// @file log10.h
/// @brief Provides a function to calculate the log10 of an unsigned integer.

#ifndef INCLUDE_NX_LOG10_H_
#define INCLUDE_NX_LOG10_H_

#include "nx/core.h"
#include "nx/constant.h"

/// @brief Library namespace.
namespace nx {
  /// @cond nx_detail
  namespace detail {
    /// @cond nx_detail_version
    namespace version {
      /// @brief 32-bit version
      template <unsigned int uVersion, class T>
      constexpr EnableIf<
        All<std::is_unsigned<T>, Bool<uVersion == 32>>,
      unsigned int> log10(T value) {
        using constant::pow10_32;
        return  (value < pow10_32[5]) ?
            (value < pow10_32[2]) ?
              (value < pow10_32[1]) ? 0 : 1
            :
            (value < pow10_32[4]) ?
              (value < pow10_32[3]) ? 2 : 3
            : 4
          :
          (value < pow10_32[7]) ?
            (value < pow10_32[6]) ? 5 : 6
          :
          (value < pow10_32[9]) ?
            (value < pow10_32[8]) ? 7 : 8
          : 9;
      }
      /// @brief 64-bit version
      template <unsigned int uVersion, class T>
      constexpr EnableIf<
        All<std::is_unsigned<T>, Bool<uVersion == 64>>,
      unsigned int> log10(T value) {
        using constant::pow10_64;
        return  (value < pow10_64[10]) ?
            log10<32>(static_cast<uint_least32_t>(value))
          :
          (value < pow10_64[15]) ?
            (value < pow10_64[12]) ?
              (value < pow10_64[11]) ? 10 : 11
            :
            (value < pow10_64[14]) ?
              (value < pow10_64[13]) ? 12 : 13
            : 14
          :
          (value < pow10_64[17]) ?
            (value < pow10_64[16]) ? 15 : 16
          :
          (value < pow10_64[19]) ?
            (value < pow10_64[18]) ? 17 : 18
          : 19;
      }
    }  // namespace version
    /// @endcond
    /// @brief [0,32]-bit selector
    template <class T>
    inline constexpr EnableIf<
        All<std::is_unsigned<T>, BitRange<T, 0, 32>>,
    unsigned int> log10(T value) {
      return version::log10<32>(value);
    }
    /// @brief [33,64]-bit selector
    template <class T>
    inline constexpr EnableIf<
        All<std::is_unsigned<T>, BitRange<T, 33, 64>>,
    unsigned int> log10(T value) {
      return version::log10<64>(value);
    }
  }  // namespace detail
  /// @endcond

  /// @brief Determines the log10 of the value.
  ///
  /// @tparam T The type of the passed value.
  /// @param value The value to examine.
  ///
  /// @return The log10 of the value, useful for determining the number of
  /// digits in a decimal number.
  template <class T>
  inline constexpr unsigned int log10(T value) {
    return detail::log10(value);
  }

}  // namespace nx

#endif  // INCLUDE_NX_LOG10_H_
