//
// Copyright (C) 2014 Jacob McIntosh <nacitar at ubercpp dot com>
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

/// @file bit_scan_reverse.h
/// @brief Provides a function to find the bit index of the most significant
/// set bit in an unsigned integral value.
/// @details If you define NX_USE_GENERIC_BIT_SCAN_REVERSE, even on platforms
/// with the appropriate compiler intrinsics, a generic fallback will be used.

#ifndef INCLUDE_NX_BIT_SCAN_REVERSE_H_
#define INCLUDE_NX_BIT_SCAN_REVERSE_H_

#include "nx/core.h"

/// @brief Library namespace.
namespace nx {

// Enable this define to not use compiler builtins.
// #define NX_USE_GENERIC_BIT_SCAN_REVERSE

#if !defined(NX_USE_GENERIC_BIT_SCAN_REVERSE) && defined(NX_TC_GCC)
// GCC BitScanReverse

/// @cond nx_detail
namespace detail {

/// @brief unsigned long long version
template <class T>
inline constexpr EnableIf<All<
    std::is_integral<T>,
    IntegerFits<T, unsigned long long>,  // NOLINT(runtime/int)
    Not<IntegerFits<T, unsigned long>>>,  // NOLINT(runtime/int)
unsigned int> BitScanReverse(T value) {
  return (value ? BitSize<T>::value - 1 - __builtin_clzll(value) : 0);
}

/// @brief unsigned long version
template <class T>
inline constexpr EnableIf<All<
    std::is_integral<T>,
    IntegerFits<T, unsigned long>,  // NOLINT(runtime/int)
    Not<IntegerFits<T, unsigned int>>>,
unsigned int> BitScanReverse(T value) {
  return (value ? BitSize<T>::value - 1 - __builtin_clzl(value) : 0);
}

/// @brief unsigned int version
template <class T>
inline constexpr EnableIf<All<
    std::is_integral<T>,
    IntegerFits<T, unsigned int>>,
unsigned int> BitScanReverse(T value) {
  return (value ? BitSize<T>::value - 1 - __builtin_clz(value) : 0);
}

}  // namespace detail
/// @endcond

#else
// Generic BitScanReverse - finds the highest set bit index

/// @cond nx_detail
namespace detail {

/// @cond nx_detail_version
namespace version {

/// @brief 8-bit version
template <unsigned int kVersion, class T>
inline constexpr EnableIf<All<
    std::is_integral<T>, Bool<kVersion == 8>>,
unsigned int> BitScanReverse(T value) {
  using constant::log_8bit;
  // for types <= 8
  return log_8bit[value];
}

/// @brief 16-bit version
template <unsigned int kVersion, class T>
inline constexpr EnableIf<All<
    std::is_integral<T>, Bool<kVersion == 16>>,
unsigned int> BitScanReverse(T value) {
  using constant::log_8bit;
  return (
    // for types <= 16
    (value >> 8) ? (8 + log_8bit[value >> 8]) : (BitScanReverse<8>(value)));
}

/// @brief 32-bit version
template <unsigned int kVersion, class T>
constexpr EnableIf<All<
    std::is_integral<T>, Bool<kVersion == 32>>,
unsigned int> BitScanReverse(T value) {
  using constant::log_8bit;
  return (
    // for types <= 32
    (value >> 16) ? (
      (value >> 24) ? (
        24 + log_8bit[value >> 24]) : (16 + log_8bit[value >> 16]))
    : (BitScanReverse<16>(value)));
}

/// @brief 64-bit version
template <unsigned int kVersion, class T>
constexpr EnableIf<All<
    std::is_integral<T>, Bool<kVersion == 64>>,
unsigned int> BitScanReverse(T value) {
  using constant::log_8bit;
  return (
    // for types <= 64
    (value >> 32) ? (
      (value >> 48) ? (
        (value >> 56) ? (
          56 + log_8bit[value >> 56]) : (48 + log_8bit[value >> 48]))
      : ((value >> 40) ? (
        40 + log_8bit[value >> 40]) : (32 + log_8bit[value >> 32])))
    : (BitScanReverse<32>(value)));
}

}  // namespace version
/// @endcond

/// @brief [0, 8]-bit selector
template <class T>
inline constexpr EnableIf<All<
    std::is_integral<T>, BitRange<T, 0, 8>>,
unsigned int> BitScanReverse(T value) {
  return version::BitScanReverse<8>(value);
}

/// @brief [9, 16]-bit selector
template <class T>
inline constexpr EnableIf<All<
    std::is_integral<T>, BitRange<T, 9, 16>>,
unsigned int> BitScanReverse(T value) {
  return version::BitScanReverse<16>(value);
}

/// @brief [17, 32]-bit selector
template <class T>
inline constexpr EnableIf<All<
    std::is_integral<T>, BitRange<T, 17, 32>>,
unsigned int> BitScanReverse(T value) {
  return version::BitScanReverse<32>(value);
}

/// @brief [33, 64]-bit selector
template <class T>
inline constexpr EnableIf<All<
    std::is_integral<T>, BitRange<T, 33, 64>>,
unsigned int> BitScanReverse(T value) {
  return version::BitScanReverse<64>(value);
}

}  // namespace detail
/// @endcond

#endif

/// @brief Determines the index of the least significant set bit.
///
/// @tparam T The type of the passed value.
/// @param value The value to examine.
///
/// @return The index of the lowest bit set, with 0 indicating the least
/// significant bit.  If the value is 0, then 0 is returned.
template <class T>
constexpr unsigned int BitScanReverse(T value) {
  return detail::BitScanReverse(value);
}

}  // namespace nx

#endif  // INCLUDE_NX_BIT_SCAN_REVERSE_H_
