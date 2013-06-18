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
/// Templates and typedefs for handling the selection of appropriate integral
/// types, includind exact-sized types as well as types in certain bit ranges.

#ifndef INCLUDE_NX_CORE_INTEGER_H_
#define INCLUDE_NX_CORE_INTEGER_H_

#ifndef OS_WINDOWS_
#include <sys/types.h>  // pid_t
#endif

#include <type_traits>
#include <limits>

#include "nx/core/os.h"
#include "nx/core/mpl.h"

namespace nx {
  namespace detail {
    template <typename T, typename Preferred, typename = void>
    struct PreferIntegralTypeInternal : public Identity<T> {
    };
    template <typename T, typename Preferred>
    struct PreferIntegralTypeInternal<
        T,
        Preferred,
        EnableIf<All<std::is_integral<T>, std::is_integral<Preferred>>>>
        : public std::conditional<
            sizeof(T) == sizeof(Preferred), Preferred, T> {
    };
    template <bool kSigned, typename T, typename = void>
    struct PreferIntegralSignInternal : public Identity<T> {
    };
    template <bool kSigned, typename T>
    struct PreferIntegralSignInternal<
        kSigned,
        T,
        EnableIf<std::is_integral<T>>>
        : public SetSigned<kSigned, T> {
    };

  }  // namespace detail

  /// If kType and kPreferred are integral and of the same size, use the
  /// kPreferred type instead of kType.
  template <typename T, typename Preferred>
  struct PreferIntegralType
      : public detail::PreferIntegralTypeInternal<T, Preferred> {
  };
  /// If kType is integral, set its sign as specified.  Use the provided
  /// type otherwise.
  template <bool kSigned, typename T>
  struct PreferIntegralSign
      : public detail::PreferIntegralSignInternal<kSigned, T> {
  };

  /// Searches for the smallest signed integral type within the specified bit
  /// range.  Provides InvalidType if no such type exists.
  template <
      bool kSigned,
      unsigned int kBitMin,
      unsigned int kBitMax = std::numeric_limits<unsigned int>::max()>
  struct IntegralLeastRangeSearch
      : public PreferIntegralSign<
          kSigned,
          Invoke<PreferIntegralType<
            Invoke<PreferIntegralType<
              Conditional<
                InRange<BitSize<char>::value, kBitMin, kBitMax>,
                char,
                Conditional<
                  InRange<
                    BitSize<short>::value,  // NOLINT(runtime/int)
                    kBitMin, kBitMax>,
                  short,  // NOLINT(runtime/int)
                  Conditional<
                    InRange<BitSize<int>::value, kBitMin, kBitMax>,
                    int,
                    Conditional<
                      InRange<
                        BitSize<long>::value,  // NOLINT(runtime/int)
                        kBitMin, kBitMax>,
                      long,  // NOLINT(runtime/int)
                      Conditional<
                        InRange<
                          BitSize<long long>::value,  // NOLINT(runtime/int)
                          kBitMin, kBitMax>,
                        long long,  // NOLINT(runtime/int)
                        InvalidType>
                    >
                  >
                >
              >,
              // prefer short over char
              short  // NOLINT(runtime/int)
            >>,
            int  // prefer int overall
          >>
        > {
  };

  /// Provides the smallest integer type in the specified bit range, with a
  /// sign if requested.  Prefers short in cases where char is the same size as
  /// a short, due to "char" being interpreted by some things as a non-numeric
  /// sort of data.  Also, prefers int in all cases where the chosen type is
  /// the same size as an int.  Using this structure directly, instead of the
  /// aliased versions, means you have to access the type member.
  template <
      bool kSigned,
      unsigned int kBitMin,
      unsigned int kBitMax = std::numeric_limits<unsigned int>::max()>
  struct IntegralLeastRangeTraits
      : public AssertValidType<
          Invoke<IntegralLeastRangeSearch<kSigned, kBitMin, kBitMax>>,
          int  // fallback type for when our static assert fails
        > {
  };

  // The following types are meant to be used in place of builtin integer
  // types and as such their names are violating standards to match the
  // convention seen in other libraries of being lowercase with underscores.

  /// Provides the smallest integer type signed as requested in the specified
  /// bit range.  A static assertion fails if none exists.
  template <
      bool kSigned,
      unsigned int kBitMin,
      unsigned int kBitMax = std::numeric_limits<unsigned int>::max()>
  using integral_least_range_t =
      Invoke<IntegralLeastRangeTraits<kSigned, kBitMin, kBitMax>>;

  /// Provides the smallest integer type signed as requested in the specified
  /// bit range, or InvalidType if none exists.
  template <
      bool kSigned,
      unsigned int kBitMin,
      unsigned int kBitMax = std::numeric_limits<unsigned int>::max()>
  using integral_least_range_search_t =
      Invoke<IntegralLeastRangeSearch<kSigned, kBitMin, kBitMax>>;

  /// Provides a signed exact-size integer type, if available.
  template <unsigned int kBits>
  using int_t = integral_least_range_t<true, kBits, kBits>;
  /// Provides the smallest signed integer type with at least the specified
  /// number of bits.
  template <unsigned int kBits>
  using int_least_t = integral_least_range_t<true, kBits>;
  /// Provides the smallest signed integer type in the specified bit range.
  template <unsigned int kBitMin, unsigned int kBitMax>
  using int_least_range_t = integral_least_range_t<true, kBitMin, kBitMax>;

  /// Provides a unsigned exact-size integer type, if available.
  template <unsigned int kBits>
  using uint_t = integral_least_range_t<false, kBits, kBits>;
  /// Provides the smallest unsigned integer type with at least the specified
  /// number of bits.
  template <unsigned int kBits>
  using uint_least_t = integral_least_range_t<false, kBits>;
  /// Provides the smallest signed integer type in the specified bit range.
  template <unsigned int kBitMin, unsigned int kBitMax>
  using uint_least_range_t = integral_least_range_t<false, kBitMin, kBitMax>;

  /// The largest available unsigned integral type.
  typedef unsigned long long uintmax_t;  // NOLINT(runtime/int)
  /// The largest available signed integral type.
  typedef signed long long intmax_t;  // NOLINT(runtime/int)

  /// An unsigned integer type 8 bits in size.
  typedef uint_t<8>  uint8_t;
  /// A signed integer type 8 bits in size.
  typedef int_t<8>   int8_t;
  /// An unsigned integer type 16 bits in size.
  typedef uint_t<16> uint16_t;
  /// A signed integer type 16 bits in size.
  typedef int_t<16>  int16_t;
  /// An unsigned integer type 32 bits in size.
  typedef uint_t<32> uint32_t;
  /// A signed integer type 32 bits in size.
  typedef int_t<32>  int32_t;
  /// An unsigned integer type 64 bits in size.
  typedef uint_t<64> uint64_t;
  /// A signed integer type 64 bits in size.
  typedef int_t<64>  int64_t;

  /// The smallest unsigned integer type at least 8 bits in size.
  typedef uint_least_t<8>  uint_least8_t;
  /// The fastest unsigned integer type at least 8 bits in size.
  typedef uint_least8_t    uint_fast8_t;
  /// The smallest signed integer type at least 8 bits in size.
  typedef int_least_t<8>   int_least8_t;
  /// The fastest signed integer type at least 8 bits in size.
  typedef int_least8_t     int_fast8_t;
  /// The smallest unsigned integer type at least 16 bits in size.
  typedef uint_least_t<16> uint_least16_t;
  /// The fastest unsigned integer type at least 16 bits in size.
  typedef uint_least16_t   uint_fast16_t;
  /// The smallest signed integer type at least 16 bits in size.
  typedef int_least_t<16>  int_least16_t;
  /// The fastest unsigned integer type at least 16 bits in size.
  typedef int_least16_t    int_fast16_t;
  /// The smallest unsigned integer type at least 32 bits in size.
  typedef uint_least_t<32> uint_least32_t;
  /// The fastest unsigned integer type at least 32 bits in size.
  typedef uint_least32_t   uint_fast32_t;
  /// The smallest signed integer type at least 32 bits in size.
  typedef int_least_t<32>  int_least32_t;
  /// The fastest unsigned integer type at least 32 bits in size.
  typedef int_least32_t    int_fast32_t;
  /// The smallest unsigned integer type at least 64 bits in size.
  typedef uint_least_t<64> uint_least64_t;
  /// The fastest unsigned integer type at least 64 bits in size.
  typedef uint_least64_t   uint_fast64_t;
  /// The smallest signed integer type at least 64 bits in size.
  typedef int_least_t<64>  int_least64_t;
  /// The fastest unsigned integer type at least 64 bits in size.
  typedef int_least64_t    int_fast64_t;

  /// An unsigned integer type of the same bit size as that of a pointer.
  typedef uint_least_t<BitSize<void*>::value> uintptr_t;
  /// A signed integer type of the same bit size as that of a pointer.
  typedef int_least_t<BitSize<void*>::value> intptr_t;

  /// A signed integer type able to represent the result of any valid pointer
  /// subtraction operation.
  typedef std::ptrdiff_t ptrdiff_t;
  /// An unsigned integer type that can store the maximum size of a
  /// theoretically possible object of any type (including array).
  typedef std::size_t size_t;

  #ifdef OS_WINDOWS_
  /// A type capable of holding a process identifier.
  typedef DWORD pid_t;
  #else
  /// A type capable of holding a process identifier.
  typedef pid_t pid_t;
  #endif
  /// An unsigned integer type of the same bit size as that of a pid_t.
  typedef uint_least_t<BitSize<pid_t>::value> uintpid_t;
  /// A signed integer type of the same bit size as that of a pid_t.
  typedef int_least_t<BitSize<pid_t>::value> intpid_t;
}  // namespace nx

#endif  // INCLUDE_NX_CORE_INTEGER_H_
