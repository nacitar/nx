/*
 * Copyright (C) 2013 Jacob McIntosh <nacitar at ubercpp dot com> 
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*! @file
 *  Templates and typedefs for handling the selection of appropriate integral
 *  types, includind exact-sized types as well as types in certain bit ranges.
 */ 

#ifndef NX_CORE_INTEGER_HPP_
#define NX_CORE_INTEGER_HPP_

#include <nx/core/os.hpp>
#include <nx/core/mpl.hpp>

#include <limits>
#include <type_traits>

#ifndef OS_WINDOWS_
#include <sys/types.h> // pid_t
#endif

namespace nx {
  namespace detail {
    template <typename T,typename Preferred,typename=void>
    struct PreferIntegralTypeInternal : public Identity<T>
    {
      static_assert(
          DependentBool<false,T>::value,
          "This only works for integral types.");
    };
    template <typename T,typename Preferred>
    struct PreferIntegralTypeInternal<
        T,
        Preferred,
        EnableIf<All<std::is_integral<T>,std::is_integral<Preferred>>>>
        : public std::conditional<
            sizeof(T) == sizeof(Preferred),
            Preferred,
            T
          > {
    };
  }

  //! If kType and kPreferred are integral and of the same size, use the
  //! kPreferred type instead of kType.
  template <typename T, typename Preferred>
  struct PreferIntegralType
      : public detail::PreferIntegralTypeInternal<T,Preferred> {
  };

  //! Provides the smallest integer type in the specified bit range, with a
  //! sign if requested.  Prefers short in cases where char is the same size as
  //! a short, due to "char" being interpreted by some things as a non-numeric
  //! sort of data.  Also, prefers int in all cases where the chosen type is
  //! the same size as an int.  Using this structure directly, instead of the
  //! aliased versions, means you have to get the type with ::type.
  template <bool kSigned,const unsigned int kBitMin,const unsigned int kBitMax>
  struct IntegralLeastRangeTraits
      : public SetSigned<
          kSigned, // Match the sign requested.
          Invoke<PreferIntegralType<
            Invoke<PreferIntegralType<
              Invoke<IsValidType<
                Conditional<
                  InRange<BitSize<char>::value,kBitMin,kBitMax>,
                  char,
                  Conditional<
                    InRange<BitSize<short>::value,kBitMin,kBitMax>,
                    short,
                    Conditional<
                      InRange<BitSize<int>::value,kBitMin,kBitMax>,
                      int,
                      Conditional<
                        InRange<BitSize<long>::value,kBitMin,kBitMax>,
                        long,
                        Conditional<
                          InRange<BitSize<long long>::value,kBitMin,kBitMax>,
                          long long,
                          InvalidType>
                      >
                    >
                  >
                >,
                int // fallback type for when our static assert fails
              >>,
              short // prefer short over char
            >>,
            int // prefer int overall
          >>
        > {
  };

  // The following types are meant to be used in place of builtin integer
  // types and as such their names are violating standards to match the
  // convention seen in other libraries of being lowercase with underscores.

  //! Provides the smallest integer type signed as requested in the specified
  //! bit range.
  template <
      bool kSigned,
      const unsigned int kBitMin,
      const unsigned int kBitMax=std::numeric_limits<unsigned int>::max()>
  using integral_least_range_t =
      Invoke<IntegralLeastRangeTraits<kSigned,kBitMin,kBitMax>>;

  //! Provides a signed exact-size integer type, if available.
  template <const unsigned int kBits>
  using int_t = integral_least_range_t<true,kBits,kBits>;
  //! Provides the smallest signed integer type with at least the specified
  //! number of bits.
  template <const unsigned int kBits>
  using int_least_t = integral_least_range_t<true,kBits>;
  //! Provides the smallest signed integer type in the specified bit range.
  template <const unsigned int kBitMin,const unsigned int kBitMax>
  using int_least_range_t = integral_least_range_t<true,kBitMin,kBitMax>;

  //! Provides a unsigned exact-size integer type, if available.
  template <const unsigned int kBits>
  using uint_t = integral_least_range_t<false,kBits,kBits>;
  //! Provides the smallest unsigned integer type with at least the specified
  //! number of bits.
  template <const unsigned int kBits>
  using uint_least_t = integral_least_range_t<false,kBits>;
  //! Provides the smallest signed integer type in the specified bit range.
  template <const unsigned int kBitMin,const unsigned int kBitMax>
  using uint_least_range_t = integral_least_range_t<false,kBitMin,kBitMax>;

  //! The largest available unsigned integral type.
  typedef unsigned long long uintmax_t;
  //! The largest available signed integral type.
  typedef signed long long intmax_t;

  typedef uint_t<8>  uint8_t;
  typedef int_t<8>   int8_t;
  typedef uint_t<16> uint16_t;
  typedef int_t<16>  int16_t;
  typedef uint_t<32> uint32_t;
  typedef int_t<32>  int32_t;
  typedef uint_t<64> uint64_t;
  typedef int_t<64>  int64_t;

  typedef uint_least_t<8>  uint_least8_t;
  typedef uint_least8_t    uint_fast8_t;
  typedef int_least_t<8>   int_least8_t;
  typedef int_least8_t     int_fast8_t;
  typedef uint_least_t<16> uint_least16_t;
  typedef uint_least16_t   uint_fast16_t;
  typedef int_least_t<16>  int_least16_t;
  typedef int_least16_t    int_fast16_t;
  typedef uint_least_t<32> uint_least32_t;
  typedef uint_least32_t   uint_fast32_t;
  typedef int_least_t<32>  int_least32_t;
  typedef int_least32_t    int_fast32_t;
  typedef uint_least_t<64> uint_least64_t;
  typedef uint_least64_t   uint_fast64_t;
  typedef int_least_t<64>  int_least64_t;
  typedef int_least64_t    int_fast64_t;

  typedef uint_least_t<BitSize<void*>::value> uintptr_t;
  typedef int_least_t<BitSize<void*>::value> intptr_t;

  typedef std::ptrdiff_t ptrdiff_t;
  typedef std::size_t size_t;

  #ifdef OS_WINDOWS_
  typedef DWORD pid_t;
  #else
  typedef pid_t pid_t;
  #endif
  typedef uint_least_t<BitSize<pid_t>::value> uintpid_t;
  typedef int_least_t<BitSize<pid_t>::value> intpid_t;
}

#endif  // NX_CORE_INTEGER_HPP_
