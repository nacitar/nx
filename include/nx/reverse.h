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

/// @file reverse.h
/// @brief Provides a function to reverse the bits of an integral value.

#ifndef INCLUDE_NX_REVERSE_H_
#define INCLUDE_NX_REVERSE_H_

#include "nx/core.h"

/// @brief Library namespace.
namespace nx {

/// @cond nx_detail
namespace detail {

/// @cond nx_detail_version
namespace version {

/// @brief 64-bit version
template <unsigned int uVersion, class T>
constexpr EnableIf<All<
    std::is_unsigned<T>, Bool<uVersion == 64>>,
T> Reverse(T value) {
  using constant::reverse_8bit;
  return
      static_cast<T>(reverse_8bit[ value        & 0xff]) << 56 |
      static_cast<T>(reverse_8bit[(value >>  8) & 0xff]) << 48 |
      static_cast<T>(reverse_8bit[(value >> 16) & 0xff]) << 40 |
      static_cast<T>(reverse_8bit[(value >> 24) & 0xff]) << 32 |
      static_cast<T>(reverse_8bit[(value >> 32) & 0xff]) << 24 |
      static_cast<T>(reverse_8bit[(value >> 40) & 0xff]) << 16 |
      static_cast<T>(reverse_8bit[(value >> 48) & 0xff]) <<  8 |
      static_cast<T>(reverse_8bit[(value >> 56) & 0xff]);
}

/// @brief 32-bit version
template <unsigned int uVersion, class T>
constexpr EnableIf<All<
    std::is_unsigned<T>, Bool<uVersion == 32>>,
T> Reverse(T value) {
  using constant::reverse_8bit;
  return
      static_cast<T>(reverse_8bit[ value        & 0xff]) << 24 |
      static_cast<T>(reverse_8bit[(value >>  8) & 0xff]) << 16 |
      static_cast<T>(reverse_8bit[(value >> 16) & 0xff]) <<  8 |
      static_cast<T>(reverse_8bit[(value >> 24) & 0xff]);
}

/// @brief 16-bit version
template <unsigned int uVersion, class T>
constexpr EnableIf<All<
    std::is_unsigned<T>, Bool<uVersion == 16>>,
T> Reverse(T value) {
  using constant::reverse_8bit;
  return
      static_cast<T>(reverse_8bit[ value        & 0xff]) <<  8 |
      static_cast<T>(reverse_8bit[(value >>  8) & 0xff]);
}

/// @brief 8-bit version
template <unsigned int uVersion, class T>
constexpr EnableIf<All<
    std::is_unsigned<T>, Bool<uVersion == 8>>,
T> Reverse(T value) {
  using constant::reverse_8bit;
  return
      static_cast<T>(reverse_8bit[value]);
}

}  // namespace version
/// @endcond

/// @brief [0,8]-bit selector
template <class T>
inline constexpr EnableIf<All<
    std::is_unsigned<T>, BitRange<T, 0, 8>>,
T> Reverse(T value) {
  return version::Reverse<8>(value);
}

/// @brief [9,16]-bit selector
template <class T>
inline constexpr EnableIf<All<
    std::is_unsigned<T>, BitRange<T, 9, 16>>,
T> Reverse(T value) {
  return version::Reverse<16>(value);
}

/// @brief [17,32]-bit selector
template <class T>
inline constexpr EnableIf<All<
    std::is_unsigned<T>, BitRange<T, 17, 32>>,
T> Reverse(T value) {
  return version::Reverse<32>(value);
}

/// @brief [33,64]-bit selector
template <class T>
inline constexpr EnableIf<All<
    std::is_unsigned<T>, BitRange<T, 33, 64>>,
T> Reverse(T value) {
  return version::Reverse<64>(value);
}

/// @brief signed-value converter
template <class T>
inline constexpr EnableIf<
    std::is_signed<T>,
T> Reverse(T value) {
  typedef typename std::make_unsigned<T>::type UT;
  return Reverse(static_cast<UT>(value));
}

}  // namespace detail
/// @endcond

/// @brief Determines the reverse of a bit pattern.
///
/// @tparam T The type of the passed value.
/// @param value The value to examine.
///
/// @return The bits of the passed value, in reverse order.
template <class T>
constexpr T Reverse(T value) {
  return detail::Reverse(value);
}

}  // namespace nx

#endif  // INCLUDE_NX_REVERSE_H_
