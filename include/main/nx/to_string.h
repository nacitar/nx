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

/// @file to_string.h
/// @brief Functions to convert integral values into strings.

#ifndef INCLUDE_NX_TO_STRING_H_
#define INCLUDE_NX_TO_STRING_H_

#include <string>
#include "nx/core.h"
#include "nx/digits.h"

/// @brief Library namespace.
namespace nx {

/// @cond nx_detail
namespace detail {

/// @brief Writes the ascii representation of an unsigned integral value into a
/// provided C-style string.  Does not terminate the string.
template <class T>
EnableIf<
    std::is_unsigned<T>,
unsigned int> ToString(T value, char*buffer, unsigned int digits = 0) {
  // Calculate the number of digits if it wasn't provided.
  if (!digits) {
    digits = Digits<10>(value);
  }
  // Point 1-past the last character
  buffer += digits;
  // Counter for digits.  Can't use digits; we must return that.
  unsigned int count = digits;
  // generate digits starting at the end of the buffer
  while (count--) {
    *(--buffer) = value % 10 + '0';  // get next digit
    value /= 10;  // remove that digit
  }
  // return number of digits
  return digits;
}

/// @brief Writes the ascii representation of a signed integral value into a
/// provided C-style string.  Does not terminate the string.
template <class T>
EnableIf<
    std::is_signed<T>,
unsigned int> ToString(T value, char*buffer, unsigned int digits = 0) {
  // Unsigned T
  typedef const typename std::make_unsigned<T>::type UT;
  if (value < 0) {
    // add the -
    *buffer = '-';
    // adjust by 1 to account for the -
    return ToString(static_cast<UT>(-value), buffer+1, digits)+1;
  }
  // just pass it on to the unsigned version
  return ToString(static_cast<UT>(value), buffer, digits);
}

/// @brief Appends the ascii representation of an unsigned integral value into
/// a provided std::string.
template <class T>
inline EnableIf<
    std::is_unsigned<T>,
unsigned int> ToString(T value, std::string*buffer, unsigned int digits = 0) {
  if (!digits) {
    digits = Digits<10>(value);
  }
  const std::string::size_type offset = buffer->size();
  // add the size of textual data
  buffer->resize(offset + digits);
  // convert the number
  return ToString(value, &((*buffer)[offset]), digits);
}

/// @brief Appends the ascii representation of a signed integral value into a
/// provided std::string.
template <class T>
EnableIf<
    std::is_signed<T>,
unsigned int> ToString(T value, std::string*buffer, unsigned int digits = 0) {
  // Unsigned T
  typedef const typename std::make_unsigned<T>::type UT;
  if (value < 0) {
    const UT abs_value = static_cast<UT>(-value);
    if (!digits) {
      digits = Digits<10>(abs_value);
    }
    const std::string::size_type offset = buffer->size();
    // add the size of textual data and the -
    buffer->resize(offset + digits + 1);
    // offset into the buffer
    char*offset_buffer=&((*buffer)[offset]);
    // add the -
    *offset_buffer = '-';
    // advance
    ++offset_buffer;
    // convert the number
    return ToString(abs_value, offset_buffer, digits);
  }
  // pass on to unsigned version
  return ToString(static_cast<UT>(value), buffer, digits);
}

/// @brief Converts an integral value into a std::string, returning it.
template <class T>
inline EnableIf<
    std::is_integral<T>,
std::string> ToString(T value, unsigned int digits = 0) {
  std::string buffer;
  ToString(value, &buffer, digits);
  return std::move(buffer);
}

}  // namespace detail
/// @endcond

/// @brief Writes a string representation of an integral value to the provided
/// character buffer.
///
/// @tparam T The type of the passed value.
/// @param buffer The location to write the string representation.
/// @param digits The digit length to be assumed for the passed value.
/// This may deviate from the actual length, if you wish to extract only the
/// least significant digits by treating it as having less.  Furthermore, this
/// can be used to zero-pad your values by specifying a greater length.
/// The default value (0) causes the function to determine the length for you.
///
/// @return The number of characters written to the buffer.
template <class T>
unsigned int ToString(T value, char*buffer, unsigned int digits = 0) {
  return detail::ToString(value, buffer, digits);
}

/// @brief Appends a string representation of an integral value to the provided
/// string.
///
/// @param buffer A pointer to the output string buffer.
/// @see ToString(T,char*,unsigned int)
///
/// @return The number of characters appended to the string.
template <class T>
unsigned int ToString(T value, std::string*buffer, unsigned int digits = 0) {
  return detail::ToString(value, buffer, digits);
}

/// @brief Converts an integral value into a string representation.
///
/// @see ToString(T,char*,unsigned int)
///
/// @return The string representation of the passed value.
template <class T>
std::string ToString(T value, unsigned int digits = 0) {
  return detail::ToString(value, digits);
}

}  // namespace nx

#endif  // INCLUDE_NX_TO_STRING_H_
