#ifndef INCLUDE_NX_CORE_SEQUENCE_H_
#define INCLUDE_NX_CORE_SEQUENCE_H_

#include "nx/core.h"
#include <initializer_list>
#include <array>


/// @brief Library namespace.
namespace nx {

/// @brief Counts the number of values at the beginning of the buffer that are
/// sequential numbers.
/// @param data A pointer/iterator to the first value to check.
/// @param end A pointer/iterator to one-past the last value to check.
/// @return For an empty buffer, 0 is returned.  Otherwise, the number of
/// sequential values is returned.  If no apparent sequence is present, 1 is
/// still returned because the first value is in a trivial sequence of length 1.
template <class Iterator>
constexpr size_t SequenceLength(const Iterator begin, const Iterator end) {
  typedef typename std::remove_const<
      typename std::remove_reference<decltype(*begin)>::type>::type T;
  if (begin != end) {
    size_t count = 1;
    Iterator data = begin;
    for (T expect = *data; ++data != end && *data == ++expect; ++count) {
    }
    //// this version for passing length instead of end
    //for (T expect = *data; count != length && *++data == ++expect; ++count) {
    //}
    return count;
  }
  return 0;
}
/// @see SequenceLength(const Iterator, const Iterator)
template <class T>
NX_FORCEINLINE constexpr size_t SequenceLength(
    const std::initializer_list<T> data) {
  return SequenceLength(data.begin(), data.end());
}
/// @see SequenceLength(const Iterator, const Iterator)
template <class T, T... values>
NX_FORCEINLINE constexpr size_t SequenceLength() {
  return nx::Size<SequenceLength<T>({values...})>::value;
}
// handle other containers
/// @see SequenceLength(const Iterator, const Iterator)
template <class Container>
NX_FORCEINLINE constexpr size_t SequenceLength(const Container& data) {
  return SequenceLength(data.begin(), data.end());
}
/// @see SequenceLength(const Iterator, const Iterator)
template <class T, std::size_t N>
NX_FORCEINLINE constexpr EnableIf<Bool<(N != 0)>,
    size_t> SequenceLength(const std::array<T, N>& data) {
  const T* const begin = &data.front();
  return SequenceLength(begin, begin + data.size());
}
/// @see SequenceLength(const Iterator, const Iterator)
template <class T, std::size_t N>
NX_FORCEINLINE constexpr EnableIf<Bool<(N == 0)>,
    size_t> SequenceLength(const std::array<T, N>& data) {
  return SequenceLength<const T*>(
      static_cast<const T*>(nullptr), static_cast<const T*>(nullptr));
}

}  // namespace nx

#endif  // INCLUDE_NX_CORE_SEQUENCE_H_
