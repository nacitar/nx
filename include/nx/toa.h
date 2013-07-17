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
/// Functions to convert integral values to strings.

#ifndef INCLUDE_NX_TOA_H_
#define INCLUDE_NX_TOA_H_

#include <string>
#include "nx/core.h"
#include "nx/log10.h"

namespace nx {
  namespace detail {
    template <class T>
    EnableIf<
      std::is_unsigned<T>,
    void> toa(T v, char*ptBuf, unsigned int uLog10) {
      ptBuf += uLog10;

      char*const ptEnd = ptBuf+1;
      // generate digits starting at the end of the buffer
      do {
        *(ptBuf--) = v % 10 + '0';  // get next digit
      } while ((v /= 10) > 0);      // delete it
      ptBuf = ptEnd;
    }
  }  // namespace detail

  // utoa
  template <class T>
  EnableIf<
    std::is_unsigned<T>,
  unsigned int> toa(T uVal, char*const ptBuf) {
    // get log10 of value
    const unsigned int uLog10 = log10(uVal);
    // convert the number
    detail::toa(uVal, ptBuf, uLog10);
    // return number of digits
    return uLog10 + 1;
  }
  // itoa
  template <class T>
  EnableIf<
    std::is_signed<T>,
  unsigned int> toa(T sVal, char*const ptBuf) {
    // Unsigned T
    typedef const typename std::make_unsigned<T>::type UT;
    if (sVal < 0) {
      // add the -
      *ptBuf = '-';
      // adjust by 1 to account for the -
      return toa(static_cast<UT>(-sVal), ptBuf+1)+1;
    }
    return toa(static_cast<UT>(sVal), ptBuf);
  }

  template <class T>
  EnableIf<
    std::is_unsigned<T>,
  unsigned int> tos(T uVal, std::string*ptBuf) {
    const std::string::size_type uOffset = ptBuf->size();
    // get size of textual data
    const unsigned int uSize = log10(uVal) + 1;
    ptBuf->resize(uOffset + uSize);
    // convert the number
    detail::toa(uVal, &((*ptBuf)[uOffset]), uSize-1);
    // return number of digits
    return uSize;
  }
  template <class T>
  EnableIf<
    std::is_signed<T>,
  unsigned int> tos(T sVal, std::string*ptBuf) {
    // Unsigned T
    typedef const typename std::make_unsigned<T>::type UT;
    if (sVal < 0) {
      // add the -
      ptBuf->push_back('-');
      // adjust by 1 to account for the -
      return tos(static_cast<UT>(-sVal), ptBuf)+1;
    }
    return tos(static_cast<UT>(sVal), ptBuf);
  }
  // returns a copy of the string
  template <class T>
  EnableIf<
    std::is_integral<T>,
  std::string> tos(T sVal) {
    std::string buffer;
    tos(sVal, &buffer);
    return buffer;
  }

}  // namespace nx

#endif  // INCLUDE_NX_TOA_H_
