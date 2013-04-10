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

#ifndef NX_CORE_MPL_HPP_
#define NX_CORE_MPL_HPP_

#include <nx/core/os.hpp>

#include <type_traits>

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

  //! Makes an integral type either signed or unsigned based upon the
  //! value of kSigned.
  template <bool kSigned,typename T>
  struct SetSigned
      : public std::conditional<
            kSigned,
            Invoke<std::make_signed<T>>,
            Invoke<std::make_unsigned<T>>
        > {
  };

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

#endif  // NX_CORE_MPL_HPP_
