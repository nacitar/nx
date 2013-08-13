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

/// @file population_count.h
/// @brief Provides a function to calculate the number of set bits in an
/// integral value.
/// @details If you define NX_USE_GENERIC_POPULATION_COUNT, even on platforms
/// with the appropriate compiler intrinsics, a generic fallback will be used.

#ifndef INCLUDE_NX_POPULATION_COUNT_H_
#define INCLUDE_NX_POPULATION_COUNT_H_

#include "nx/core.h"

/// @brief Library namespace.
namespace nx {

// Enable this define to not use compiler builtins.
// #define NX_USE_GENERIC_POPULATION_COUNT

/// @cond nx_detail
namespace detail {

#if !defined(NX_USE_GENERIC_POPULATION_COUNT) && defined(NX_TC_GCC)
// GCC PopulationCount - counts the number of set bits

/// @brief unsigned long long selector
template <class T>
inline constexpr EnableIf<All<
    std::is_integral<T>,
    IntegerFits<T, unsigned long long>,  // NOLINT(runtime/int)
    Not<IntegerFits<T, unsigned long>>>,  // NOLINT(runtime/int)
unsigned int> PopulationCount(T value) {
  return __builtin_popcountll(value);
}

/// @brief unsigned long selector
template <class T>
inline constexpr EnableIf<All<
    std::is_integral<T>,
    IntegerFits<T, unsigned long>,  // NOLINT(runtime/int)
    Not<IntegerFits<T, unsigned int>>>,
unsigned int> PopulationCount(T value) {
  return __builtin_popcountl(value);
}

/// @brief unsigned int selector
template <class T>
inline constexpr EnableIf<All<
    std::is_integral<T>,
    IntegerFits<T, unsigned int>>,
unsigned int> PopulationCount(T value) {
  return __builtin_popcount(value);
}

#else

/// @cond nx_detail_version
namespace version {
// Generic PopulationCount

/// @brief 64-bit version
template <unsigned int uVersion, class T>
constexpr EnableIf<All<
    std::is_unsigned<T>, Bool<uVersion == 64>>,
unsigned int> PopulationCount(T value) {
  using nx::constant::population_count_8bit;
  return static_cast<unsigned int>(
    population_count_8bit[ value        & 0xff]) +
    population_count_8bit[(value >> 8)  & 0xff]  +
    population_count_8bit[(value >> 16) & 0xff]  +
    population_count_8bit[(value >> 24) & 0xff]  +
    population_count_8bit[(value >> 32) & 0xff]  +
    population_count_8bit[(value >> 40) & 0xff]  +
    population_count_8bit[(value >> 48) & 0xff]  +
    population_count_8bit[(value >> 56) & 0xff];
}

/// @brief 32-bit version
template <unsigned int uVersion, class T>
constexpr EnableIf<All<
    std::is_unsigned<T>, Bool<uVersion == 32>>,
unsigned int> PopulationCount(T value) {
  using nx::constant::population_count_8bit;
  return static_cast<unsigned int>(
    population_count_8bit[ value        & 0xff]) +
    population_count_8bit[(value >> 8)  & 0xff]  +
    population_count_8bit[(value >> 16) & 0xff]  +
    population_count_8bit[(value >> 24) & 0xff];
}

/// @brief 16-bit version
template <unsigned int uVersion, class T>
constexpr EnableIf<All<
    std::is_unsigned<T>, Bool<uVersion == 16>>,
unsigned int> PopulationCount(T value) {
  using nx::constant::population_count_8bit;
  return static_cast<unsigned int>(
    population_count_8bit[ value        & 0xff]) +
    population_count_8bit[(value >> 8)  & 0xff];
}

/// @brief 8-bit version
template <unsigned int uVersion, class T>
constexpr EnableIf<All<
    std::is_unsigned<T>, Bool<uVersion == 8>>,
unsigned int> PopulationCount(T value) {
  using nx::constant::population_count_8bit;
  return static_cast<unsigned int>(population_count_8bit[value]);
}

}  // namespace version
/// @endcond

/// @brief [0,8]-bit selector
template <class T>
inline constexpr EnableIf<All<
    std::is_unsigned<T>, BitRange<T, 0, 8>>,
unsigned int> PopulationCount(T value) {
  return detail::PopulationCount<8>(value);
}

/// @brief [9,16]-bit selector
template <class T>
inline constexpr EnableIf<All<
    std::is_unsigned<T>, BitRange<T, 9, 16>>,
unsigned int> PopulationCount(T value) {
  return detail::PopulationCount<16>(value);
}

/// @brief [17,32]-bit selector
template <class T>
inline constexpr EnableIf<All<
    std::is_unsigned<T>, BitRange<T, 17, 32>>,
unsigned int> PopulationCount(T value) {
  return detail::PopulationCount<32>(value);
}

/// @brief [33,64]-bit selector
template <class T>
inline constexpr EnableIf<All<
    std::is_unsigned<T>, BitRange<T, 33, 64>>,
unsigned int> PopulationCount(T value) {
  return detail::PopulationCount<64>(value);
}

/// @brief signed-value converter
template <class T>
inline constexpr EnableIf<
    std::is_signed<T>,
unsigned int> PopulationCount(T value) {
  typedef typename std::make_unsigned<T>::type UT;
  return PopulationCount(static_cast<UT>(value));
}

#endif

}  // namespace detail
/// @endcond

/// @brief Determines the number of set bits.
///
/// @tparam T The type of the passed value.
/// @param value The value to examine.
///
/// @return The number of bits set.
template <class T>
constexpr unsigned int PopulationCount(T value) {
  return detail::PopulationCount(value);
}

}  // namespace nx

#endif  // INCLUDE_NX_POPULATION_COUNT_H_
