#ifndef INCLUDE_NX_CORE_SHIFT_H_
#define INCLUDE_NX_CORE_SHIFT_H_

#include "nx/core.h"
#include "nx/core/integer.h"
#include "util.h"

/// @brief Library namespace.
namespace nx {

/// @cond nx_detail
namespace detail {

template <class T>
using ShiftFunction = Function<T, T, ssize_t>;

template <class T>
NX_FORCEINLINE constexpr T ShiftTooFar(T value, ssize_t kOffset) {
  // shifting >= sizeof type, just zeroing it out.  It would be UB if the shift
  // was attempted anyway, and logically if it did work it would make sense to
  // result in 0.
  return 0;
}
template <class T>
NX_FORCEINLINE constexpr T ShiftZero(T value, ssize_t kOffset) {
  return value;
}
template <class T>
NX_FORCEINLINE constexpr T ShiftRight(T value, ssize_t kOffset) {
  return static_cast<T>(static_cast<MakeUnsigned<T>>(value) >> kOffset);
}
template <class T>
NX_FORCEINLINE constexpr T ShiftLeft(T value, ssize_t kOffset) {
  return static_cast<T>(static_cast<MakeUnsigned<T>>(value) << -kOffset);
}
// Intentionally not inlined.  If calling this function in a context where it
// won't be optimized out completely, it's better to not inline it.
template <class T>
/*NX_FORCEINLINE*/ constexpr ShiftFunction<T> ShiftVersion(ssize_t kOffset) {
  if (kOffset == 0) {
    return ShiftZero;
  } else if (kOffset > 0 && kOffset < static_cast<ssize_t>(Bits<T>::Size())) {
    return ShiftRight;
  } else if (kOffset < 0 && kOffset > -static_cast<ssize_t>(Bits<T>::Size())) {
    return ShiftLeft;
  }
  return ShiftTooFar;
}

// This is essentially here as a second measure in case the constexpr support
// of the compiler doesn't work properly.
// TODO(nacitar): confirm constexpr support and remove this? Continue not
// trusting it and use this as a compilation-time verification of support?
// Does the standard say it WILL inline the constexpr call, or that it CAN?
template <class T, ssize_t kOffset>
using ShiftVersionConstant =
    Constant<ShiftFunction<T>, ShiftVersion<T>(kOffset)>;

}  // namespace detail
/// @endcond


/// @brief Shifts the provided value by the specified amount.  A negative
/// offset causes left shift, and a positive offset causes a right shift.
/// @details All shifts are performed as unsigned shifts.
/// @tparam kValue The value to shift.
/// @tparam kOffset The shift offset.
/// @return The result.
template <class T, T kValue, ssize_t kOffset>
NX_FORCEINLINE constexpr T Shift() {
  return Constant<T,
      detail::ShiftVersionConstant<T, kOffset>::value(kValue, kOffset)>::value;
}
/// @brief Shifts the provided value by the specified amount.  A negative
/// offset causes left shift, and a positive offset causes a right shift.
/// @details All shifts are performed as unsigned shifts.
/// @tparam kOffset The shift offset.
/// @param value The value to shift.
/// @return The result.
template <ssize_t kOffset, class T>
NX_FORCEINLINE constexpr T Shift(T value) {
  return detail::ShiftVersionConstant<T, kOffset>::value(value, kOffset);
}
/// @brief Shifts the provided value by the specified amount.  A negative
/// offset causes left shift, and a positive offset causes a right shift.
/// @details All shifts are performed as unsigned shifts.
/// @tparam kValue The value to shift.
/// @param offset The shift offset.
/// @return The result.
template <class T, T kValue>
NX_FORCEINLINE constexpr T Shift(ssize_t offset) {
  return detail::ShiftVersion<T>(offset)(kValue, offset);
}
/// @brief Shifts the provided value by the specified amount.  A negative
/// offset causes left shift, and a positive offset causes a right shift.
/// @details All shifts are performed as unsigned shifts.
/// @param value The value to shift.
/// @param offset The shift offset.
/// @return The result.
template <class T>
NX_FORCEINLINE constexpr T Shift(T value, ssize_t offset) {
  return detail::ShiftVersion<T>(offset)(value, offset);
}


/// @cond nx_detail
namespace detail {

// If the mask touches the edge before shifting, and after shifting, no need to
// and it with the mask.
template <ssize_t kOffset, class T, T kMask>
NX_FORCEINLINE constexpr EnableIf<Bool<(
    IsEdgeBitMask<T, kMask>() &&
    IsEdgeBitMask<T, Shift<T, kMask, kOffset>()>() && kOffset)>,
    T> ShiftMask(T value) {
  return Shift<kOffset>(value);
}

template <ssize_t kOffset, class T, T kMask>
NX_FORCEINLINE constexpr EnableIf<Not<Bool<(
    IsEdgeBitMask<T, kMask>() &&
    IsEdgeBitMask<T, Shift<T, kMask, kOffset>()>() && kOffset)>>,
    T> ShiftMask(T value) {
  return Shift<kOffset>(value & kMask);
}


}  // namespace detail
/// @endcond

/// @brief Behaves like Shift(), but also performs a bitwise-and of the value
/// and the provided mask prior to shifting ONLY if necessary, avoiding it if
/// the zero-bits that would be shifted in would sufficiently cover it.
/// @details There is no situation where this optimization would make sense if
/// either the mask or the offset were not known at compile-time.  If it is not
/// known, just perform the bitwise-and directly and pass the result to Shift().
/// Adding versions of this function to allow non compilation-time-constants
/// would do nothing but allow the caller to generate inefficient code.
/// @tparam kOffset The shift offset.
/// @tparam kMask The mask for bitwise-and with the value.
/// @param value The value to shift.
template <ssize_t kOffset, class T, T kMask>
NX_FORCEINLINE constexpr T ShiftMask(T value) {
  return detail::ShiftMask<kOffset, T, kMask>(value);
}

/// @brief Behaves like Shift(), but also performs a bitwise-and of the value
/// and the provided mask prior to shifting ONLY if necessary, avoiding it if
/// the zero-bits that would be shifted in would sufficiently cover it.
/// @details There is no situation where this optimization would make sense if
/// either the mask or the offset were not known at compile-time.  If it is not
/// known, just perform the bitwise-and directly and pass the result to Shift().
/// Adding versions of this function to allow non compilation-time-constants
/// would do nothing but allow the caller to generate inefficient code.
/// @tparam kValue The value to shift.
/// @tparam kOffset The shift offset.
/// @tparam kMask The mask for bitwise-and with the value.
template <class T, T kValue, ssize_t kOffset, T kMask>
NX_FORCEINLINE constexpr T ShiftMask() {
  return Constant<T, ShiftMask<kOffset, T, kMask>(kValue)>::value;
}

}  // namespace nx

#endif  // INCLUDE_NX_CORE_SHIFT_H_
