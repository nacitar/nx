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
 *  A collection of metaprogramming tools. 
 */ 

#ifndef NX_MPL_HPP_
#define NX_MPL_HPP_

#include <limits> // numeric_limits
#include <type_traits>

#ifndef OS_WINDOWS_
#include <sys/types.h> // pid_t
#endif
// TODO: need windows header

// It is suggested that any types that use a static-assertion-using class be
// implemented not as alias templates but rather as actual classes.  That way,
// stack traces are more meaningful.
namespace nx {
  //! Simple wrapper around an integral constant.  Can be used to make a value
  //! depend upon a template parameter by passing the types as additional
  //! arguments to the template.
  template<typename T, T kValue,typename...>
  struct DependentIntegralConstant : public std::integral_constant<T,kValue> {
  };

  // No need to invoke integral constants... they resolve to themselves!

  //! Meta-constant boolean
  template <bool kValue>
  struct Bool : public std::integral_constant<bool,kValue> {
  };
  //! Meta-constant char
  template <char kValue>
  struct Char : public std::integral_constant<char,kValue> {
  };
  //! Meta-constant short
  template <short kValue>
  struct Short : public std::integral_constant<short,kValue> {
  };
  //! Meta-constant int
  template <int kValue>
  struct Int : public std::integral_constant<int,kValue> {
  };
  //! Meta-constant long
  template <long kValue>
  struct Long : public std::integral_constant<long,kValue> {
  };
  //! Meta-constant long long
  template <long long kValue>
  struct LongLong : public std::integral_constant<long long,kValue> {
  };
  //! Meta-constant unsigned char
  template <char kValue>
  struct UChar : public std::integral_constant<unsigned char,kValue> {
  };
  //! Meta-constant unsigned short
  template <unsigned short kValue>
  struct UShort : public std::integral_constant<unsigned short,kValue> {
  };
  //! Meta-constant unsigned int
  template <unsigned int kValue>
  struct UInt : public std::integral_constant<unsigned int,kValue> {
  };
  //! Meta-constant unsigned long
  template <unsigned long kValue>
  struct ULong : public std::integral_constant<unsigned long,kValue> {
  };
  //! Meta-constant unsigned long long
  template <unsigned long long kValue>
  struct ULongLong : public std::integral_constant<unsigned long long,kValue> {
  };
  //! A dependent boolean type
  template <bool kValue, typename... T>
  struct DependentBool : public DependentIntegralConstant<bool, kValue, T...> {
  };

  //! Alias for removing typename and ::type from boilerplate.
  template <typename T>
  using Invoke = typename T::type;

  //! Basic identity metafunction; provides the type unaltered.
  //! Useful for passing raw types to templates expecting ::type.
  template <typename T>
  struct Identity {
    using type = T;
  };

  //! Alias to get the conditional of something with a ::value member
  template <typename If, typename Then, typename Else>
  using Conditional = Invoke<std::conditional<If::value, Then, Else>>;
  
  //! Meta-logical negation (Not)
  template <typename T>
  struct Not : public Bool<!T::value> {
  };

  //! Meta-logical disjunction (Or)
  template <typename... T>
  struct Any : Bool<false> {
  };
  template <typename Head, typename... Tail>
  struct Any<Head, Tail...> : Conditional<Head, Bool<true>, Any<Tail...>> {
  };

  //! Meta-logical conjunction (And)
  template <typename... T>
  struct All : public Bool<true> {
  };
  template <typename Head, typename... Tail>
  struct All<Head, Tail...> : Conditional<Head, All<Tail...>, Bool<false>> {
  };

  //! A version of enable_if that takes a trait and resolves itself
  template <typename Condition,typename T=void>
  using EnableIf = Invoke<std::enable_if<Condition::value,T>>;

  //! A negated version of enable_if that takes a trait and resolves itself
  template <typename Condition,typename T=void>
  using DisableIf = Invoke<std::enable_if<Not<Condition>::value,T>>;

  //! An distinct "invalid" type, useful for metaprogramming.
  struct InvalidType {
  };

  //! Checks if the type is a valid one via a static assertion.
  template <typename T, typename Fallback=void>
  struct IsValidType : public Identity<T> {
  };

  //! Specialization to detect InvalidType.  In order to minimize compilation
  //! errors when the assertion fails, you can pass a fallback type that is
  //! likely to help in that endeavor.
  template <typename Fallback>
  struct IsValidType<InvalidType,Fallback> : public Identity<Fallback> { 
    static_assert(
        DependentBool<false,Fallback>::value,
        "No type exists that fulfills the specified requirements.");
  };

  //! Stores the size of the provided type in bits.
  template <class T>
  struct BitSize : public UInt<sizeof(T)*CHAR_BIT> {
  };

  //! Stores a true value if kValue is in the range [kMin,kMax]
  template <unsigned int kValue,unsigned int kMin,unsigned int kMax>
  struct InRange : public Bool<kMin <= kValue && kValue <= kMax> {
  };

  //! Makes an integral type either signed or unsigned based upon the value of
  //! kSigned.
  template <bool kSigned,typename T>
  struct SetSigned
      : public std::conditional<
            kSigned,
            Invoke<std::make_signed<T>>,
            Invoke<std::make_unsigned<T>>
        > {
  };

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

  //! Provides the smallest integer type in the specified bit range, with a sign
  //! if requested.  Prefers short in cases where sizeof(char) == sizeof(short)
  //! due to "char" being interpreted by some things as a non-numeric sort of
  //! data.  Also, prefers int in all cases where the chosen type is the same
  //! size as an int.  Using this structure directly, instead of the aliased
  //! versions, means you have to get the type with ::type.
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

  //! The following types are meant to be used in place of builtin integer types
  //! and as such their names are violating standards to match the convention
  //! seen in other libraries of being lowercase with underscores.

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

  //! Instantiates to be the specified number of bytes in size.
  template<const unsigned int kSize>
  class SpecificSize {
    unsigned char buffer_[kSize];
  };

  //! Instantiates to be exactly the same number of bytes as an object of type T
  //! would require on the stack.  Used to reserve space for a type without
  //! constructing it.  This allows much neater code in such cases.
  //! new same_size<T>[50]; instead of using something like
  //! new unsigned char[sizeof(T)*50];
  template<class T>
  class SameSize : public SpecificSize<sizeof(T)> {
  };
}

#endif  // NX_MPL_HPP_

#include <iostream>
int main(int /*argc*/,char* /*argv*/ [])
{
  int*pt=nullptr;
  nx::int_t<32> poop;
  nx::int_least_t<32> poop2;
  
  std::cout << "int: " << nx::BitSize<int>() << std::endl;

  return 0;
}

