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

/// @file toa.h
/// @brief Functions to convert integral values into strings.

#ifndef INCLUDE_NX_TOA_H_
#define INCLUDE_NX_TOA_H_

#include <string>
#include "nx/core.h"
#include "nx/digits.h"

/// @brief Library namespace.
namespace nx {

  /// @cond nx_detail
  namespace detail {

    /// @brief Writes the ascii representation of an unsigned integral value
    /// into a provided C-style string.  Does not terminate the string.
    template <class T>
    EnableIf<
      std::is_unsigned<T>,
    unsigned int> toa(T value, char*buffer, unsigned int num_digits = 0) {
      // Calculate the number of digits if it wasn't provided.
      if (!num_digits) {
        num_digits = digits<10>(value);
      }
      // Point 1-past the last character
      buffer += num_digits;
      // Counter for digits.  Can't use num_digits; we must return that.
      unsigned int count = num_digits;
      // generate digits starting at the end of the buffer
      while (count--) {
        *(--buffer) = value % 10 + '0';  // get next digit
        value /= 10;  // remove that digit
      }
      // return number of digits
      return num_digits;
    }

    /// @brief Writes the ascii representation of a signed integral value
    /// into a provided C-style string.  Does not terminate the string.
    template <class T>
    EnableIf<
      std::is_signed<T>,
    unsigned int> toa(T value, char*buffer, unsigned int num_digits = 0) {
      // Unsigned T
      typedef const typename std::make_unsigned<T>::type UT;
      if (value < 0) {
        // add the -
        *buffer = '-';
        // adjust by 1 to account for the -
        return toa(static_cast<UT>(-value), buffer+1, num_digits)+1;
      }
      // just pass it on to the unsigned version
      return toa(static_cast<UT>(value), buffer, num_digits);
    }

    /// @brief Appends the ascii representation of an unsigned integral value
    /// into a provided std::string.
    template <class T>
    inline EnableIf<
      std::is_unsigned<T>,
    unsigned int> tos(
        T value, std::string*buffer, unsigned int num_digits = 0) {
      if (!num_digits) {
        num_digits = digits<10>(value);
      }
      const std::string::size_type offset = buffer->size();
      // add the size of textual data
      buffer->resize(offset + num_digits);
      // convert the number
      return toa(value, &((*buffer)[offset]), num_digits);
    }

    /// @brief Appends the ascii representation of a signed integral value
    /// into a provided std::string.
    template <class T>
    EnableIf<
      std::is_signed<T>,
    unsigned int> tos(
        T value, std::string*buffer, unsigned int num_digits = 0) {
      // Unsigned T
      typedef const typename std::make_unsigned<T>::type UT;
      if (value < 0) {
        const UT abs_value = static_cast<UT>(-value);
        if (!num_digits) {
          num_digits = digits<10>(abs_value);
        }
        const std::string::size_type offset = buffer->size();
        // add the size of textual data and the -
        buffer->resize(offset + num_digits + 1);
        // offset into the buffer
        char*offset_buffer=&((*buffer)[offset]);
        // add the -
        *offset_buffer = '-';
        // advance
        ++offset_buffer;
        // convert the number
        return toa(abs_value, offset_buffer, num_digits);
      }
      // pass on to unsigned version
      return tos(static_cast<UT>(value), buffer, num_digits);
    }

    /// @brief Converts an integral value into a std::string, returning it.
    template <class T>
    inline EnableIf<
      std::is_integral<T>,
    std::string> tos(T value, unsigned int num_digits = 0) {
      std::string buffer;
      tos(value, &buffer, num_digits);
      return std::move(buffer);
    }
  }  // namespace detail
  /// @endcond

  /// @brief Writes a string representation of an integral value to the
  /// provided character buffer.
  ///
  /// @tparam T The type of the passed value.
  /// @param buffer The location to write the string representation.
  /// @param num_digits The digit length to be assumed for the passed value.
  /// This may deviate from the actual length, if you wish to extract only the
  /// least significant digits by treating it as having less.  Furthermore,
  /// this can be used to zero-pad your values by specifying a greater length.
  /// The default value (0) causes the function to determine the length for you.
  ///
  /// @return The number of characters written to the buffer.
  template <class T>
  unsigned int toa(T value, char*buffer, unsigned int num_digits = 0) {
    return detail::toa(value, buffer, num_digits);
  }

  /// @brief Appends a string representation of an integral value to the
  /// provided string.
  ///
  /// @param buffer A pointer to the output string buffer.
  /// @see toa(T,char*,unsigned int)
  ///
  /// @return The number of characters appended to the string.
  template <class T>
  inline unsigned int tos(
      T value, std::string*buffer, unsigned int num_digits = 0) {
    return detail::tos(value, buffer, num_digits);
  }

  /// @brief Converts an integral value into a string representation.
  ///
  /// @see toa(T,char*,unsigned int)
  ///
  /// @return The string representation of the passed value.
  template <class T>
  inline std::string tos(T value, unsigned int num_digits = 0) {
    return detail::tos(value, num_digits);
  }

}  // namespace nx

#endif  // INCLUDE_NX_TOA_H_
