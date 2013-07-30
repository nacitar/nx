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

/// @file popcount.h
/// @brief Provides a function to calculate the number of set bits in an
/// integral value.
/// @details If you define NX_USE_GENERIC_POPCOUNT, even on platforms with
/// the appropriate compiler intrinsics, a generic fallback will be used.

#ifndef INCLUDE_NX_POPCOUNT_H_
#define INCLUDE_NX_POPCOUNT_H_

#include "nx/core.h"

/// @brief Library namespace.
namespace nx {
  // Enable this define to not use compiler builtins.
  // #define NX_USE_GENERIC_POPCOUNT

  #if !defined(NX_USE_GENERIC_POPCOUNT) && defined(NX_TC_GCC)
  // GCC PopCount - counts the number of set bits

  /// @cond nx_detail
  namespace detail {
    /// @brief unsigned long long selector
    template <class T>
    inline constexpr EnableIf<
        All<
          std::is_integral<T>,
          IntegerFits<T, unsigned long long>,  // NOLINT(runtime/int)
          Not<IntegerFits<T, unsigned long>>  // NOLINT(runtime/int)
        >,
    unsigned int> PopCount(T value) {
      return __builtin_popcountll(value);
    }
    /// @brief unsigned long selector
    template <class T>
    inline constexpr EnableIf<
        All<
          std::is_integral<T>,
          IntegerFits<T, unsigned long>,  // NOLINT(runtime/int)
          Not<IntegerFits<T, unsigned int>>
        >,
    unsigned int> PopCount(T value) {
      return __builtin_popcountl(value);
    }
    /// @brief unsigned int selector
    template <class T>
    inline constexpr EnableIf<
        All<
          std::is_integral<T>,
          IntegerFits<T, unsigned int>
        >,
    unsigned int> PopCount(T value) {
      return __builtin_popcount(value);
    }
  }
  /// @endcond
  #else
  /// @cond nx_detail
  namespace detail {
    /// @cond nx_detail_version
    namespace version {
      // Generic PopCount
      /// @brief 64-bit version
      template <unsigned int uVersion, class T>
      constexpr EnableIf<
        All<std::is_unsigned<T>, Bool<uVersion == 64>>,
      unsigned int> PopCount(T value) {
        using nx::constant::PopCount256;
        return static_cast<unsigned int>(
          PopCount256[ value        & 0xff]) +
          PopCount256[(value >> 8 ) & 0xff]  +
          PopCount256[(value >> 16) & 0xff]  +
          PopCount256[(value >> 24) & 0xff]  +
          PopCount256[(value >> 32) & 0xff]  +
          PopCount256[(value >> 40) & 0xff]  +
          PopCount256[(value >> 48) & 0xff]  +
          PopCount256[(value >> 56) & 0xff];
      }
      /// @brief 32-bit version
      template <unsigned int uVersion, class T>
      constexpr EnableIf<
        All<std::is_unsigned<T>, Bool<uVersion == 32>>,
      unsigned int> PopCount(T value) {
        using nx::constant::PopCount256;
        return static_cast<unsigned int>(
          PopCount256[ value        & 0xff]) +
          PopCount256[(value >> 8 ) & 0xff]  +
          PopCount256[(value >> 16) & 0xff]  +
          PopCount256[(value >> 24) & 0xff];
      }
      /// @brief 16-bit version
      template <unsigned int uVersion, class T>
      constexpr EnableIf<
        All<std::is_unsigned<T>, Bool<uVersion == 16>>,
      unsigned int> PopCount(T value) {
        using nx::constant::PopCount256;
        return static_cast<unsigned int>(
          PopCount256[ value        & 0xff]) +
          PopCount256[(value >> 8 ) & 0xff];
      }
      /// @brief 8-bit version
      template <unsigned int uVersion, class T>
      constexpr EnableIf<
        All<std::is_unsigned<T>, Bool<uVersion == 8>>,
      unsigned int> PopCount(T value) {
        using nx::constant::PopCount256;
        return static_cast<unsigned int>(PopCount256[value]);
      }
    }  // namespace version
    /// @endcond
    /// @brief [0,8]-bit selector
    template <class T>
    inline constexpr EnableIf<
        All<std::is_unsigned<T>, BitRange<T, 0, 8>>,
    unsigned int> PopCount(T value) {
      return detail::PopCount<8>(value);
    }
    /// @brief [9,16]-bit selector
    template <class T>
    inline constexpr EnableIf<
        All<std::is_unsigned<T>, BitRange<T, 9, 16>>,
    unsigned int> PopCount(T value) {
      return detail::PopCount<16>(value);
    }
    /// @brief [17,32]-bit selector
    template <class T>
    inline constexpr EnableIf<
        All<std::is_unsigned<T>, BitRange<T, 17, 32>>,
    unsigned int> PopCount(T value) {
      return detail::PopCount<32>(value);
    }
    /// @brief [33,64]-bit selector
    template <class T>
    inline constexpr EnableIf<
        All<std::is_unsigned<T>, BitRange<T, 33, 64>>,
    unsigned int> PopCount(T value) {
      return detail::PopCount<64>(value);
    }
    /// @brief signed-value converter
    template <class T>
    inline constexpr EnableIf<
        std::is_signed<T>,
    unsigned int> PopCount(T value) {
      typedef typename std::make_unsigned<T>::type UT;
      return PopCount(static_cast<UT>(value));
    }
  }  // namespace detail
  /// @endcond
  #endif
  /// @brief Determines the number of set bits.
  ///
  /// @tparam T The type of the passed value.
  /// @param value The value to examine.
  ///
  /// @return The number of bits set.
  template <class T>
  inline constexpr unsigned int PopCount(T value) {
    return detail::PopCount(value);
  }
}  // namespace nx

#endif  // INCLUDE_NX_POPCOUNT_H_
