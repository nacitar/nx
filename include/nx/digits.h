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

/// @file digits.h
/// @brief Provides a function to count the digits of an integer.
/// @todo If for some reason all types are above 64-bit, you should be able to
/// optionally truncate the value down to 64-bits and use those versions.

#ifndef INCLUDE_NX_DIGITS_H_
#define INCLUDE_NX_DIGITS_H_

#include <cstdlib>  // integral std::abs
#include <cmath>  // float/double std::abs

#include "nx/core.h"
#include "nx/constant.h"


/// @brief Library namespace.
namespace nx {

  /// @cond nx_detail
  namespace detail {

    /// @cond nx_detail_version
    namespace version {

      /// @brief 32-bit version
      template <unsigned int number_base, unsigned int version, class T>
      constexpr EnableIf<All<
          std::is_unsigned<T>,
          Bool<number_base == 10>,
          Bool<version == 32>>,
      unsigned int> Digits(T value) {
          return  (value < Power<T, 10, 5>::value) ?
              (value < Power<T, 10, 2>::value) ?
                (value < Power<T, 10, 1>::value) ? 1 : 2
              :
              (value < Power<T, 10, 4>::value) ?
                (value < Power<T, 10, 3>::value) ? 3 : 4
              : 5
            :
            (value < Power<T, 10, 7>::value) ?
              (value < Power<T, 10, 6>::value) ? 6 : 7
            :
            (value < Power<T, 10, 9>::value) ?
              (value < Power<T, 10, 8>::value) ? 8 : 9
            : 10;
      }

      /// @brief 64-bit version
      template <unsigned int number_base, unsigned int version, class T>
      constexpr EnableIf<All<
          std::is_unsigned<T>,
          Bool<number_base == 10>,
          Bool<version == 64>>,
      unsigned int> Digits(T value) {
        return  (value < Power<T, 10, 10>::value) ?
            Digits<number_base, 32>(value)
          :
          (value < Power<T, 10, 15>::value) ?
            (value < Power<T, 10, 12>::value) ?
              (value < Power<T, 10, 11>::value) ? 11 : 12
            :
            (value < Power<T, 10, 14>::value) ?
              (value < Power<T, 10, 13>::value) ? 13 : 14
            : 15
          :
          (value < Power<T, 10, 17>::value) ?
            (value < Power<T, 10, 16>::value) ? 16 : 17
          :
          (value < Power<T, 10, 19>::value) ?
            (value < Power<T, 10, 18>::value) ? 18 : 19
          : 20;
      }

      /// @brief Version for large types, no longer constexpr
      template <unsigned int number_base, unsigned int version, class T>
      EnableIf<All<
          std::is_unsigned<T>,
          Bool<number_base == 10>,
          Bool<version == 0>>,
      unsigned int> Digits(T value) {
        constexpr const T next_pow10 = Power<T, 10, 20>::value;
        if (value < next_pow10) {
          return version::Digits<number_base, version>(value);
        }
        value /= next_pow10;
        unsigned int count = 21;
        while (value) {
          ++count;
          value /= 10u;
        }
        return count;
      }

      /// @brief Signed value forwarder for constexpr handled types.
      template <unsigned int number_base, unsigned int version, class T>
      inline constexpr EnableIf<All<
          std::is_signed<T>,
          Bool<number_base == 10>,
          Bool<version != 0>>,
      unsigned int> Digits(T value) {
        typedef const typename std::make_unsigned<T>::type UT;
        return Digits<number_base, version>(
            static_cast<UT>((value < 0?-value:value)) );
      }

      /// @brief Signed value forwarder for larger types.
      template <unsigned int number_base, unsigned int version, class T>
      inline EnableIf<All<
          std::is_signed<T>,
          Bool<number_base == 10>,
          Bool<version == 0>>,
      unsigned int> Digits(T value) {
        typedef const typename std::make_unsigned<T>::type UT;
        return Digits<number_base, version>(
            static_cast<UT>((value < 0?-value:value)) );
      }

    }  // namespace version
    /// @endcond

    /// @brief [0,32]-bit selector
    template <unsigned int number_base, class T>
    inline constexpr EnableIf<All<
        Bool<number_base == 10>,
        BitRange<T, 0, 32>>,
    unsigned int> Digits(T value) {
      return version::Digits<number_base, 32>(value);
    }

    /// @brief [33,64]-bit selector
    template <unsigned int number_base, class T>
    inline constexpr EnableIf<All<
        Bool<number_base == 10>,
        BitRange<T, 33, 64>>,
    unsigned int> Digits(T value) {
      return version::Digits<number_base, 64>(value);
    }

    /// @brief Large generic selector
    template <unsigned int number_base, class T>
    inline EnableIf<All<
        Bool<number_base == 10>,
        BitRange<T, 65>>,
    unsigned int> Digits(T value) {
      return version::Digits<number_base, 0>(value);
    }

  }  // namespace detail
  /// @endcond

  /// @brief Determines the digits of the value.
  /// @details This version handles types we can compute as constexpr.
  /// Slightly faster for unsigned types.
  ///
  /// @tparam T The type of the passed value.
  /// @param value The value to examine.
  ///
  /// @return The digits of the value, useful for determining the number of
  /// digits in a decimal number.  This does not count any negative sign.
  template <unsigned int number_base = 10, class T>
  constexpr EnableIf<
    BitRange<T, 0, 64>,
  unsigned int> Digits(T value) {
    return detail::Digits<number_base>(value);
  }

  /// @brief Determines the digits of the value.
  /// @details This version handles types we can't compute as constexpr
  /// because they are too large.  Slightly faster for unsigned types.
  ///
  /// @tparam T The type of the passed value.
  /// @param value The value to examine.
  ///
  /// @return The digits of the value, useful for determining the number of
  /// digits in a decimal number.  This does not count any negative sign.
  template <unsigned int number_base = 10, class T>
  EnableIf<
    BitRange<T, 65>,
  unsigned int> Digits(T value) {
    return detail::Digits<number_base>(value);
  }

}  // namespace nx

#endif  // INCLUDE_NX_DIGITS_H_
