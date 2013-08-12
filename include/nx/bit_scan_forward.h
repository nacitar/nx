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

/// @file bit_scan_forward.h
/// @brief Provides a function to find the bit index of the least significant
/// set bit in an unsigned integral value.
/// @details If you define NX_USE_GENERIC_BIT_SCAN_FORWARD, even on platforms
/// with the appropriate compiler intrinsics, a generic fallback will be used.

#ifndef INCLUDE_NX_BIT_SCAN_FORWARD_H_
#define INCLUDE_NX_BIT_SCAN_FORWARD_H_

#include "nx/core.h"

/// @brief Library namespace.
namespace nx {
  // Enable this define to not use compiler builtins.
  // #define NX_USE_GENERIC_BIT_SCAN_FORWARD

  #if !defined(NX_USE_GENERIC_BIT_SCAN_FORWARD) && defined(NX_TC_GCC)
  // GCC BitScanForward - finds the lowest set bit index
  /// @cond nx_detail
  namespace detail {
    /// @brief unsigned long long version
    template <class T>
    inline constexpr EnableIf<
        All<
          std::is_integral<T>,
          IntegerFits<T, unsigned long long>,  // NOLINT(runtime/int)
          Not<IntegerFits<T, unsigned long>>  // NOLINT(runtime/int)
        >,
    unsigned int> BitScanForward(T value) {
      return (value ? __builtin_ctzll(value) : 0);
    }
    /// @brief unsigned long version
    template <class T>
    inline constexpr EnableIf<
        All<
          std::is_integral<T>,
          IntegerFits<T, unsigned long>,  // NOLINT(runtime/int)
          Not<IntegerFits<T, unsigned int>>
        >,
    unsigned int> BitScanForward(T value) {
      return (value ? __builtin_ctzl(value) : 0);
    }
    /// @brief unsigned int version
    template <class T>
    inline constexpr EnableIf<
        All<
          std::is_integral<T>,
          IntegerFits<T, unsigned int>
        >,
    unsigned int> BitScanForward(T value) {
      return (value ? __builtin_ctz(value) : 0);
    }
  }  // namespace detail
  /// @endcond
  #else
  // Generic BitScanForward
  /// @cond nx_detail
  namespace detail {
    /// @cond nx_detail_version
    namespace version {
      /// @brief 64-bit version
      template <unsigned int kVersion, class T>
      inline constexpr EnableIf<
        All<std::is_integral<T>, Bool<kVersion == 64>>,
      unsigned int> BitScanForward(T value) {
        typedef typename std::make_signed<
          typename std::add_const<T>::type
        >::type const_signed_T;
        typedef typename std::make_unsigned<
          typename std::add_const<T>::type
        >::type const_unsigned_T;
        return constant::de_bruijn_64bit[
            (
              (
                static_cast<const_unsigned_T>(
                  value & -static_cast<const_signed_T>(value))
                * constant::de_bruijn_multiplier_64bit)
              >> 58u)
            & 0x3Fu];
      }
      /// @brief 32-bit version
      template <unsigned int kVersion, class T>
      inline constexpr EnableIf<
        All<std::is_integral<T>, Bool<kVersion == 32>>,
      unsigned int> BitScanForward(T value) {
        typedef typename std::make_signed<
          typename std::add_const<T>::type
        >::type const_signed_T;
        typedef typename std::make_unsigned<
          typename std::add_const<T>::type
        >::type const_unsigned_T;
        return constant::de_bruijn_32bit[
            (
              (
                static_cast<const_unsigned_T>(
                  value & -static_cast<const_signed_T>(value))
                * constant::de_bruijn_multiplier_32bit)
              >> 27u)
            & 0x1Fu];
      }
    }  // namespace version
    /// @endcond
    /// @brief [0,32]-bit selector
    template <class T>
    inline constexpr EnableIf<
        All<std::is_integral<T>, BitRange<T, 0, 32>>,
    unsigned int> BitScanForward(T value) {
      return version::BitScanForward<32>(value);
    }
    /// @brief [33,64]-bit selector
    template <class T>
    inline constexpr EnableIf<
        All<std::is_integral<T>, BitRange<T, 33, 64>>,
    unsigned int> BitScanForward(T value) {
      return version::BitScanForward<64>(value);
    }
  }  // namespace detail
  /// @endcond
  #endif

  /// @brief Determines the index of the most significant set bit.
  ///
  /// @tparam T The type of the passed value.
  /// @param value The value to examine.
  ///
  /// @return The index of the highest bit set, with 0 indicating the least
  /// significant bit.  If the value is 0, then 0 is returned.
  template <class T>
  inline constexpr unsigned int BitScanForward(T value) {
    return detail::BitScanForward(value);
  }
}  // namespace nx
#endif  // INCLUDE_NX_BIT_SCAN_FORWARD_H_
