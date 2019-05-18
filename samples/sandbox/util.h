#ifndef INCLUDE_NX_CORE_UTIL_H_
#define INCLUDE_NX_CORE_UTIL_H_

#include "nx/core.h"

/// @brief Library namespace.
namespace nx {

/// @brief Checks the criteria that if any bits are set, they are all
/// contiguous and also include the LSB.
/// @details This criteria is met by 0.
/// @param value A value to examine.
/// @return true if the criteria is met.
template <class T>
NX_FORCEINLINE constexpr bool IsLowBitMask(T value) {
  return ((value & (value + 1)) == 0);
}
/// @brief Checks the criteria that if any bits are set, they are all
/// contiguous and also include the LSB.
/// @details This criteria is met by 0.
/// @tparam kValue A value to examine.
/// @return true if the criteria is met.
template <class T, T kValue>
NX_FORCEINLINE constexpr bool IsLowBitMask() {
  return Constant<T, IsLowBitMask(kValue)>::value;
}
/// @brief Checks the criteria that if any bits are set, they are all
/// contiguous and also include the MSB.
/// @details This criteria is met by 0.
/// @param value A value to examine.
/// @return true if the criteria is met.
template <class T>
NX_FORCEINLINE constexpr bool IsHighBitMask(T value) {
  return IsLowBitMask(~value);
}
/// @brief Checks the criteria that if any bits are set, they are all
/// contiguous and also include the MSB.
/// @details This criteria is met by 0.
/// @tparam kValue A value to examine.
/// @return true if the criteria is met.
template <class T, T kValue>
NX_FORCEINLINE constexpr bool IsHighBitMask() {
  return Constant<T, IsHighBitMask(kValue)>::value;
}
/// @brief Checks the criteria that if any bits are set, they are all
/// contiguous and also include either the LSB or the MSB (or both).
/// @details This criteria is met by 0.
/// @param value A value to examine.
/// @return true if the criteria is met.
template <class T>
NX_FORCEINLINE constexpr bool IsEdgeBitMask(T value) {
  return (IsLowBitMask(value) || IsHighBitMask(value));
}
/// @brief Checks the criteria that if any bits are set, they are all
/// contiguous and also include either the LSB or the MSB (or both).
/// @details This criteria is met by 0.
/// @tparam kValue A value to examine.
/// @return true if the criteria is met.
template <class T, T kValue>
NX_FORCEINLINE constexpr bool IsEdgeBitMask() {
  return Constant<T, IsEdgeBitMask(kValue)>::value;
}

}  // namespace nx

#endif  // INCLUDE_NX_CORE_UTIL_H_
