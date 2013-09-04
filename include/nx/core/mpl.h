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

/// @file mpl.h
/// @brief A collection of metaprogramming tools.

#ifndef INCLUDE_NX_CORE_MPL_H_
#define INCLUDE_NX_CORE_MPL_H_


#include <type_traits>
#include <limits>

#include "nx/core/os.h"

/// @brief Preprocessor text concatenation.
#define NX_PP_CAT(x, y) NX_PP_CAT1(x, y)

/// @brief Preprocessor text concatenation helper.
#define NX_PP_CAT1(x, y) x##y

/// @brief Simple static warning.
#define NX_STATIC_WARNING(cond, msg) \
struct NX_PP_CAT(static_warning, __LINE__) { \
  NX_DEPRECATED(void _(::std::false_type const&), msg) {} \
  void _(::std::true_type const&) {} \
  NX_PP_CAT(static_warning, __LINE__)() {_(::nx::Bool<cond>());} \
}

/// @brief Static warning at template level.
/// Note: using NX_STATIC_WARNING_TEMPLATE changes the meaning of a program in
/// a small way.  It introduces a member/variable declaration.  This means at
/// least one byte of space in each structure/class instantiation.
/// NX_STATIC_WARNING should be preferred in any non-template situation.
#define NX_STATIC_WARNING_TEMPLATE(cond, msg) \
    NX_STATIC_WARNING(cond, msg) NX_PP_CAT(_localvar_staticwarning, __LINE__)

/// @brief When placed within the private section of a class/structure this
/// will disallow the default constructor.
#define NX_INCONSTRUCTIBLE(TypeName) \
  TypeName() = delete

/// @brief When placed within the private section of a class/structure this
/// will disallow the move constructor.
#define NX_IMMOVABLE(TypeName) \
  TypeName(TypeName&&) = delete

/// @brief When placed within the private section of a class/structure this
/// will disallow the copy constructor and the assignment operator.
#define NX_NONCOPYABLE(TypeName) \
  TypeName(const TypeName&) = delete;   \
  void operator=(const TypeName&) = delete

/// @brief When placed in the private section of a class/structure, this
/// will disallow the default, move, and copy constructors as well as the
/// assignment operator.
/// @see NX_INCONSTRUCTIBLE
/// @see NX_IMMOVABLE
/// @see NX_NONCOPYABLE
#define NX_ABSTRACT(TypeName) \
  NX_INCONSTRUCTIBLE(TypeName); \
  NX_IMMOVABLE(TypeName); \
  NX_NONCOPYABLE(TypeName)

// It is suggested that any types that use a static-assertion-using class be
// implemented not as alias templates but rather as actual classes.  That way,
// gcc's error messages are more meaningful.

/// @brief Library namespace.
namespace nx {

/// @brief Simple wrapper around an integral constant.  Can be used to make a
/// value depend upon a template parameter by passing the types as additional
/// arguments to the template.
template<typename T, T kValue, typename...>
class DependentIntegralConstant : public std::integral_constant<T, kValue> {
};

/// @brief A dependent boolean type
template <bool kValue, typename... T>
class DependentBool : public DependentIntegralConstant<bool, kValue, T...> {
};

// No need to invoke integral constants... they resolve to themselves!

/// @brief Meta-constant boolean
template <bool kValue>
class Bool : public std::integral_constant<bool, kValue> {
};

/// @brief Meta-constant int
template <int kValue>
class Int : public std::integral_constant<int, kValue> {
};

/// @brief Meta-constant unsigned int
template <unsigned int kValue>
class UInt : public std::integral_constant<unsigned int, kValue> {
};

/// @brief Alias for removing typename and type members from boilerplate.
template <typename T>
using Invoke = typename T::type;

/// @brief Basic identity metafunction; provides the type unaltered. Useful for
/// passing raw types to templates expecting a type member.
template <typename T>
class Identity {
 public:
  /// @brief The type provided as a template argument.
  typedef T type;
 private:
  NX_NONCOPYABLE(Identity);
};

/// @brief Alias to get the conditional of something with a value member.
template <typename If, typename Then, typename Else>
using Conditional = Invoke<std::conditional<If::value, Then, Else>>;

/// @brief Meta-logical negation (Not)
template <typename T>
class Not : public Bool<!T::value> {
};

/// @brief Meta-logical disjunction (Or)
template <typename... T>
class Any : public Bool<false> {
};

/// @brief Specialization for checking the truth of one condition before
/// chaining to check the other conditions.
template <typename Head, typename... Tail>
class Any<Head, Tail...> : public Conditional<Head, Bool<true>, Any<Tail...>> {
};

/// @brief Meta-logical conjunction (And)
template <typename... T>
class All : public Bool<true> {
};

/// @brief Specialization for checking the truth of one condition before
/// chaining to check the other conditions.
template <typename Head, typename... Tail>
class All<Head, Tail...> : public Conditional<
    Head, All<Tail...>, Bool<false>> {
};

/// @brief A version of enable_if that takes a trait and resolves itself
template <typename Condition, typename T = void>
using EnableIf = Invoke<std::enable_if<Condition::value, T>>;

/// @brief A negated version of enable_if that takes a trait and resolves
/// itself.
template <typename Condition, typename T = void>
using DisableIf = Invoke<std::enable_if<Not<Condition>::value, T>>;

/// @brief An distinct "invalid" type, useful for metaprogramming.
class InvalidType {
 private:
  NX_ABSTRACT(InvalidType);
};

/// @brief Checks if the provided type is valid, and if so provides it.
/// Otherwise providing the Fallback type.  If kAssert is true, a static
/// assertion failure will also occur upon an invalid type.
template <bool kAssert, typename T, typename Fallback = T>
class CheckValidType : public Bool<true> {
 public:
  typedef T type;
};

/// @brief Specialization that fails a static assertion on invalid types.
template <typename Fallback>
class CheckValidType<true, InvalidType, Fallback> : public Bool<false> {
 public:
  typedef Fallback type;
  static_assert(
      DependentBool<false, Fallback>::value,
      "No type exists that fulfills the specified requirements.");
};

/// @brief Specialization that does not fail a static assertion on invalid
/// types.
template <typename Fallback>
class CheckValidType<false, InvalidType, Fallback> : public Bool<false> {
 public:
  typedef Fallback type;
};

/// @brief Shorthand for CheckValidType with static assertions.  Using this
/// will make the presence of assertions more clear to the reader.
template <typename T, typename Fallback = T>
class AssertValidType : public CheckValidType<true, T, Fallback> {
};

/// @brief Shorthand for CheckValidType without static assertions.
template <typename T, typename Fallback = T>
class IsValidType : public CheckValidType<false, T, Fallback> {
};

/// @brief Stores the size of the provided type in bits.
template <typename T>
class BitSize : public UInt<sizeof(T)*CHAR_BIT> {
};

/// @cond nx_detail
namespace detail {

template <
    typename T, unsigned int kBits, bool kAllowPartial, class Enable = void>
class BitMaskInternal : public std::integral_constant<
    T, (static_cast<T>(1) << kBits)-1> {
};

template <typename T, unsigned int kBits, bool kAllowPartial>
class BitMaskInternal<
    T, kBits, kAllowPartial,
    EnableIf<Not<std::is_integral<T>>>> : public UInt<0> {
  static_assert(
      DependentBool<false, T>::value, "The provided type is not integral.");
};

template <typename T, unsigned int kBits, bool kAllowPartial>
class BitMaskInternal<
    T, kBits, kAllowPartial,
    EnableIf<All<std::is_integral<T>, Bool<(kBits == BitSize<T>::value)>>>>
    : public std::integral_constant<T, ~static_cast<T>(0)> {
};

// If we allow partial masks, we just max out what bits we have if we can't
// hold them all.
template <typename T, unsigned int kBits>
class BitMaskInternal<
    T, kBits, true,
    EnableIf<All<std::is_integral<T>, Bool<(kBits > BitSize<T>::value)>>>>
    : public std::integral_constant<T, ~static_cast<T>(0)> {
};

// If we don't allow partial masks, we fail a static assert if we can't hold
// all the bits.
template <typename T, unsigned int kBits>
class BitMaskInternal<
    T, kBits, false,
    EnableIf<All<std::is_integral<T>, Bool<(kBits > BitSize<T>::value)>>>>
    : public std::integral_constant<T, 0> {
  static_assert(
      DependentBool<false, T>::value,
      "This type does not have enough bits to hold a mask of this size.");
};

}  // namespace detail
/// @endcond

/// @brief Provides a bit mask of type T with the lowest kBits bits set.
template <typename T, unsigned int kBits, bool kAllowPartial = false>
class BitMask : public detail::BitMaskInternal<T, kBits, kAllowPartial> {
};

/// @brief Stores a true value if kValue is in the range [kMin,kMax]
template <unsigned int kValue, unsigned int kMin, unsigned int kMax>
class InRange : public Bool< (kMin <= kValue && kValue <= kMax)> {
};

/// @brief Checks if the size of the type T is within the requested range.
template <
  typename T,
  unsigned int kMin,
  unsigned int kMax = std::numeric_limits<unsigned int>::max()>
class BitRange : public InRange<BitSize<T>::value, kMin, kMax> {
};

/// @brief Determines if integer type T is <= the size of integer type
/// Destination.
template <typename T, typename Destination>
class IntegerFits
    : public All<
          std::is_integral<T>,
          std::is_integral<Destination>,
          InRange<BitSize<T>::value, 0, BitSize<Destination>::value>> {
};

/// @brief Makes an integral type either signed or unsigned based upon the
/// value of kSigned.
template <bool kSigned, typename T>
class SetSigned : public std::conditional<
    kSigned,
    Invoke<std::make_signed<T>>,
    Invoke<std::make_unsigned<T>>> {
 private:
  NX_ABSTRACT(SetSigned);
};

/// @brief Determines if multiplying kLHS with kRHS will result in an overflow.
template <class T, T kLHS, T kRHS>
class OverflowMult
    : public Bool<(kRHS != 0 && kLHS > std::numeric_limits<T>::max() / kRHS)> {
};

/// @brief Instantiates to be the specified number of bytes in size.
template<unsigned int kSize>
class SpecificSize {
  unsigned char buffer_[kSize];
};

/// @brief Instantiates to be exactly the same number of bytes as an object
/// of type T would require on the stack.  Used to reserve space for a type
/// without constructing it.  This allows much neater code in such cases.
/// new same_size<T>[50]; instead of using something like
/// new unsigned char[sizeof(T)*50];
template<class T>
class SameSize : public SpecificSize<sizeof(T)> {
};

/// @cond nx_detail
namespace detail {

template <
    class T, T kBase, unsigned int kPower, class Enable = void>
class Power {
 public:
  static constexpr T previous = Power<T, kBase, kPower-1>::value;
  static constexpr T value = kBase * previous;
  static_assert(
      !OverflowMult<T, kBase, previous>::value,
      "Value overflows type.");
};

template <class T, T kBase, unsigned int kPower>
class Power<T, kBase, kPower, EnableIf< Bool<kPower == 0>>> {
 public:
  static constexpr T value = 1;
};

}  // namespace detail
/// @endcond

/// @todo When http://gcc.gnu.org/bugzilla/show_bug.cgi?id=58059 is fixed,
/// implement a specialization for handling overflow so you get only one
/// error message for it.
/// @brief Determines kBase to the power of kPower.
template <class T, T kBase, unsigned int kPower>
class Power : public detail::Power<T, kBase, kPower> {
};

}  // namespace nx

#endif  // INCLUDE_NX_CORE_MPL_H_
