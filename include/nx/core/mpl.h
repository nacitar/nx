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
/// A collection of metaprogramming tools.

#ifndef INCLUDE_NX_CORE_MPL_H_
#define INCLUDE_NX_CORE_MPL_H_


#include <type_traits>

#include "nx/core/os.h"

// It is suggested that any types that use a static-assertion-using class be
// implemented not as alias templates but rather as actual classes.  That way,
// gcc's error messages are more meaningful.
namespace nx {
  /// Simple wrapper around an integral constant.  Can be used to make a value
  /// depend upon a template parameter by passing the types as additional
  /// arguments to the template.
  template<typename T, T kValue, typename...>
  struct DependentIntegralConstant : public std::integral_constant<T, kValue> {
  };

  /// A dependent boolean type
  template <bool kValue, typename... T>
  struct DependentBool : public DependentIntegralConstant<bool, kValue, T...> {
  };

  // No need to invoke integral constants... they resolve to themselves!

  /// Meta-constant boolean
  template <bool kValue>
  struct Bool : public std::integral_constant<bool, kValue> {
  };
  /// Meta-constant int
  template <int kValue>
  struct Int : public std::integral_constant<int, kValue> {
  };
  /// Meta-constant unsigned int
  template <unsigned int kValue>
  struct UInt : public std::integral_constant<unsigned int, kValue> {
  };

  /// Alias for removing typename and type members from boilerplate.
  template <typename T>
  using Invoke = typename T::type;

  /// Basic identity metafunction; provides the type unaltered.
  /// Useful for passing raw types to templates expecting a type member.
  template <typename T>
  struct Identity {
    /// The type provided as a template argument.
    using type = T;
  };

  /// Alias to get the conditional of something with a value member.
  template <typename If, typename Then, typename Else>
  using Conditional = Invoke<std::conditional<If::value, Then, Else>>;

  /// Meta-logical negation (Not)
  template <typename T>
  struct Not : public Bool<!T::value> {
  };

  /// Meta-logical disjunction (Or)
  template <typename... T>
  struct Any : Bool<false> {
  };
  template <typename Head, typename... Tail>
  struct Any<Head, Tail...> : Conditional<Head, Bool<true>, Any<Tail...>> {
  };

  /// Meta-logical conjunction (And)
  template <typename... T>
  struct All : public Bool<true> {
  };
  template <typename Head, typename... Tail>
  struct All<Head, Tail...> : Conditional<Head, All<Tail...>, Bool<false>> {
  };

  /// A version of enable_if that takes a trait and resolves itself
  template <typename Condition, typename T = void>
  using EnableIf = Invoke<std::enable_if<Condition::value, T>>;

  /// A negated version of enable_if that takes a trait and resolves itself
  template <typename Condition, typename T = void>
  using DisableIf = Invoke<std::enable_if<Not<Condition>::value, T>>;

  /// An distinct "invalid" type, useful for metaprogramming.
  struct InvalidType {
  };

  /// Checks if the provided type is valid, and if so provides it.  Otherwise
  /// providing the Fallback type.  If kAssert is true, a static assertion
  /// failure will also occur upon an invalid type.
  template <bool kAssert, typename T, typename Fallback = T>
  struct CheckValidType : public Identity<T>, Bool<true> {
    using Identity<T>::type;
  };

  /// Specialization that fails a static assertion on invalid types.
  template <typename Fallback>
  struct CheckValidType<
      true,
      InvalidType,
      Fallback>
      : public Identity<Fallback>, Bool<false> {
    using Identity<Fallback>::type;
    static_assert(
        DependentBool<false, Fallback>::value,
        "No type exists that fulfills the specified requirements.");
  };

  /// Specialization that does not fail a static assertion on invalid types.
  template <typename Fallback>
  struct CheckValidType<
      false,
      InvalidType,
      Fallback>
      : public Identity<Fallback>, Bool<false> {
        using Identity<Fallback>::type;
  };

  /// Shorthand for CheckValidType with static assertions.  Using this will
  /// make the presence of assertions more clear to the reader.
  template <typename T, typename Fallback = T>
  struct AssertValidType : public CheckValidType<true, T, Fallback> {
  };

  /// Shorthand for CheckValidType without static assertions.
  template <typename T, typename Fallback = T>
  struct IsValidType : public CheckValidType<false, T, Fallback> {
  };

  /// Stores the size of the provided type in bits.
  template <typename T>
  struct BitSize : public UInt<sizeof(T)*CHAR_BIT> {
  };

  namespace detail {
    template <
        typename T,
        unsigned int kBits,
        bool kAllowPartial,
        class Enable = void>
    struct BitMaskInternal
        : public std::integral_constant<
            T, (static_cast<T>(1) << kBits)-1> {
    };

    template <typename T, unsigned int kBits, bool kAllowPartial>
    struct BitMaskInternal<
        T,
        kBits,
        kAllowPartial,
        EnableIf<Not<std::is_integral<T>>>>
        : public UInt<0> {
      static_assert(
          DependentBool<false, T>::value,
          "The provided type is not integral.");
    };
    template <typename T, unsigned int kBits, bool kAllowPartial>
    struct BitMaskInternal<
        T,
        kBits,
        kAllowPartial,
        EnableIf<All<
            std::is_integral<T>,
            Bool<(kBits == BitSize<T>::value)>>>>
        : public std::integral_constant<T, ~static_cast<T>(0)> {
    };
    // If we allow partial masks, we just max out what bits we have if we can't
    // hold them all.
    template <typename T, unsigned int kBits>
    struct BitMaskInternal<
        T,
        kBits,
        true,
        EnableIf<All<
            std::is_integral<T>,
            Bool<(kBits > BitSize<T>::value)>>>>
        : public std::integral_constant<T, ~static_cast<T>(0)> {
    };
    // If we don't allow partial masks, we fail a static assert if we can't
    // hold all the bits.
    template <typename T, unsigned int kBits>
    struct BitMaskInternal<
        T,
        kBits,
        false,
        EnableIf<All<
            std::is_integral<T>,
            Bool<(kBits > BitSize<T>::value)>>>>
        : public std::integral_constant<T, 0> {
      static_assert(
          DependentBool<false, T>::value,
          "This type does not have enough bits to hold a mask of this size.");
    };
  }  // namespace detail

  /// Provides a bit mask of type T with the lowest kBits bits set.
  template <typename T, unsigned int kBits, bool kAllowPartial = false>
  struct BitMask : public detail::BitMaskInternal<T, kBits, kAllowPartial> {
  };

  /// Stores a true value if kValue is in the range [kMin,kMax]
  template <unsigned int kValue, unsigned int kMin, unsigned int kMax>
  struct InRange : public Bool< (kMin <= kValue && kValue <= kMax)> {
  };

  /// Makes an integral type either signed or unsigned based upon the
  /// value of kSigned.
  template <bool kSigned, typename T>
  struct SetSigned
      : public std::conditional<
            kSigned,
            Invoke<std::make_signed<T>>,
            Invoke<std::make_unsigned<T>>
        > {
  };

  /// Instantiates to be the specified number of bytes in size.
  template<unsigned int kSize>
  class SpecificSize {
    unsigned char buffer_[kSize];
  };

  /// Instantiates to be exactly the same number of bytes as an object of type T
  /// would require on the stack.  Used to reserve space for a type without
  /// constructing it.  This allows much neater code in such cases.
  /// new same_size<T>[50]; instead of using something like
  /// new unsigned char[sizeof(T)*50];
  template<class T>
  class SameSize : public SpecificSize<sizeof(T)> {
  };
}  // namespace nx

#endif  // INCLUDE_NX_CORE_MPL_H_
