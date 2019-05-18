#include "nx/core.h"
#include "nx/application.h"
#include <iostream>

#include <array>
#include <list>
#include <vector>
#include <initializer_list>

#include "shift.h"
#include "sequence.h"

#include <iterator>

/// @brief Library namespace.
namespace nx {

template <class Key, class Value, size_t MaxSize>
class Map {
public:
  typedef std::pair<Key, Value> value_type; // public
private:
  value_type underlying_data_[MaxSize];
  size_t size_;
public:
  constexpr Map(std::initializer_list<value_type> values) : size_(0) {
    for (const value_type& value : values) {
      operator[](value.first) = value.second;
    }
  }

  constexpr void erase(size_t index) {
    if (size_) {
      while (++index < size_) {
        // TODO: shouldn't be necessary; why isn't operator= constexpr?
        value_type& previous = underlying_data_[index - 1];
        value_type& current = underlying_data_[index];
        previous.first = current.first;
        previous.second = current.second;
        //underlying_data_[index - 1] = underlying_data_[index];
      }
      --size_;
    }
  }

  constexpr const value_type* data() const {
    return underlying_data_;
  }

  constexpr Value& operator[](const Key& key) {
    size_t low = 0;
    size_t high = size_;
    while (low < high) {
      size_t mid = low + ((high - low) / 2);
      if (underlying_data_[mid].first < key) {
        low = mid+1;
      } else if (key < underlying_data_[mid].first) {
        high = mid;
      } else {
        return underlying_data_[mid].second;
      }
    }
    if (size_ == MaxSize) {
      throw std::length_error("Map has already reached maximum size.");
    }
    // only if adding
    ++size_;
    for (size_t index = size_; index > low; --index) {
      // TODO: shouldn't be necessary; why isn't operator= constexpr?
      value_type& previous = underlying_data_[index - 1];
      value_type& current = underlying_data_[index];
      current.first = previous.first;
      current.second = previous.second;
      //underlying_data_[index] = underlying_data_[index - 1];
    }
    value_type& element = underlying_data_[low];
    element.first = key;
    element.second = Value();
    return element.second;
  }
};

template <class Iterator>
using ValueType = typename std::iterator_traits<Iterator>::value_type;


// UNINSTANTIABLE
template <class Iterator>
class FieldTraits {
public:
  typedef typename std::iterator_traits<Iterator>::value_type value_type;
  static constexpr NX_FORCEINLINE value_type Mask(size_t offset, size_t length) {
    return static_cast<value_type>(Shift(
        Bits<MakeUnsigned<value_type>>::LowMask(
            static_cast<unsigned int>(length)),
        -static_cast<ssize_t>(offset)));
  }

  static constexpr NX_FORCEINLINE ssize_t ShiftDelta(size_t offset, value_type index) {
    return (static_cast<ssize_t>(offset) - static_cast<ssize_t>(index));
  }

  static constexpr NX_FORCEINLINE std::tuple<value_type, ssize_t> toField(
      Iterator begin, Iterator end, value_type processedMask = 0,
      size_t offset = 0) {
    value_type mask = 0;
    size_t next_offset = 0;
    Iterator it = begin;
    if (it != end) {
      value_type expect = *it;
      const ssize_t shiftDelta = ShiftDelta(offset, expect);
      for (;;) {
        size_t length = 1;
        while (++it != end) {
          const value_type value = *it;
          if (value != ++expect) {
            expect = value;
            break; // put the mask in
          }
          ++length;
        }
        mask |= Mask(offset, length);
        offset += length;
        if (next_offset == 0) {
          next_offset = offset;
        }
      }
      return std::make_tuple(mask, shiftDelta, next_offset);
    }
  }
};


template <class T>
class OffsetMask {
  size_t offset_;
  T mask_;
public:
  constexpr OffsetMask(size_t offset, T mask)
      : offset_(offset), mask_(mask) {
  }
  constexpr void setOffset(size_t offset) {
    offset_ = offset;
  }
  constexpr size_t offset() {
    return offset_;
  }
  constexpr void setMask(T mask) {
    mask_ = mask;
  }
  constexpr T mask() {
    return mask_;
  }
};

template <class T, size_t N>
struct Array {
  T underlying_data_[N]; // must be public; aggregate type.

  constexpr T* data() {
    return underlying_data_;
  }
  constexpr const T* data() const {
    return underlying_data_;
  }

  constexpr T& operator[](size_t index) {
    return underlying_data_[index];
  }
  constexpr const T& operator[](size_t index) const {
    return underlying_data_[index];
  }

  constexpr size_t size() const {
    return N;
  }
};

constexpr auto foo(size_t value) {
  //size_t data[1] =  { 0 };
  //data[0] = value;
  //std::array<size_t, 1> data {{ 0 }};
  OffsetMask<size_t> obj(0, value);
  obj.setOffset(value + 1);
  return obj;
}
  
#if 0
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
#endif

template <class Iterator>
constexpr std::tuple<ValueType<Iterator>, ssize_t> makeMask(Iterator begin, Iterator end, size_t index = 0) {
  typedef FieldTraits<Iterator> Traits;
  typedef typename Traits::value_type T;
  Iterator it = begin;

  size_t length = SequenceLength(it, end);
  if (length) {
    const ssize_t shiftDelta = Traits::ShiftDelta(index, *it);
    T mask = Traits::Mask(index, length);
    while (true) {
      it += length;
      if (it == end) {
        break;
      }
      index += length;
      if (Traits::ShiftDelta(index, *it) == shiftDelta) {
        length = SequenceLength(it, end);
        mask |= Traits::Mask(index, length);
      }
    }
    return std::make_tuple(mask, shiftDelta);
  }
  return std::make_tuple(static_cast<T>(0), static_cast<ssize_t>(0));;
}

template <class Iterator>
ValueType<Iterator> toField(const Iterator begin, const Iterator end, ValueType<Iterator> value) {
  typedef decltype(value) T;
  // The index of the middle element of the array, indicating a 0 shift.

  T result = 0;
  ssize_t position = 0;
  ssize_t sequenceLength = 0;
  Iterator it = begin;
  while ((sequenceLength = static_cast<ssize_t>(SequenceLength(it, end))) != 0) {
    const T mask = Shift(Bits<T>::LowMask(static_cast<unsigned int>(sequenceLength)),
        -static_cast<ssize_t>(position));
    const ssize_t shiftDelta = position - static_cast<ssize_t>(*it);
    result |= Shift(value & mask, shiftDelta);
    it += static_cast<ssize_t>(sequenceLength);
  }
  return result;
}

template <class T>
class ShiftTraits {
  const T mask_;
  const size_t offset_;
  public:
  constexpr ShiftTraits(T mask, size_t offset)
      : mask_(mask), offset_(offset) {
  }
};

}  // namespace nx

constexpr int test(int value) {
  nx::Map<int, int, 5> x { {1, 2}, {3,4} };
  x[1] = value;
  return x[1];
}
/// @brief The class for the Sandbox application.
class SandboxApplication : public nx::Application {
 public:
  int main() {
    const std::vector<int> vec{1, 2, 5, 6};
    const std::vector<int> vec2{1, 4, 5, 6};
    const std::vector<int> vec3{};
    const std::vector<int> vec4{1};
    const std::vector<int> vec5{1, 2};
    const std::vector<int> vec6{1, 2, 3 ,4};
    const std::list<int> lst{1, 2, 5, 6};
    
    nx::Map<int, int, 5> data = { {5, 2}, {3, 1} };
    std::cout << data[5] << std::endl;
    data[5] = 7;
    std::cout << data[5] << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << data[3] << std::endl;
    data[3] = 7;
    std::cout << data[3] << std::endl;
    std::cout << std::endl;
    std::cout << data[4] << std::endl;
    data[4] = 7;
    std::cout << data[4] << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << nx::Int<test(5)>::value << std::endl;
    std::cout << std::endl;
    std::cout << nx::SequenceLength(vec.begin(), vec.end()) << std::endl;
    std::cout << nx::Int<nx::SequenceLength<int, 1, 2, 5, 6>()>::value << std::endl;
    std::cout << nx::Int<nx::SequenceLength(std::array<int, 4>{{1, 2, 5, 6}})>::value << std::endl;
    std::cout << nx::Int<nx::SequenceLength({1, 2, 5, 6})>::value << std::endl;
    std::cout << nx::SequenceLength(vec) << std::endl;
    std::cout << nx::SequenceLength(vec2) << std::endl;
    std::cout << nx::SequenceLength(vec3) << std::endl;
    std::cout << nx::SequenceLength(vec4) << std::endl;
    std::cout << nx::SequenceLength(vec5) << std::endl;
    std::cout << nx::SequenceLength(vec6) << std::endl;
    std::cout << nx::SequenceLength(lst) << std::endl;
    std::cout << nx::SequenceLength<int>() << std::endl;
    std::cout << std::endl;

    std::cout << nx::Int<nx::Shift<-3>(7)>::value << std::endl;
    std::cout << nx::Int<nx::Shift<-3, int>(7)>::value << std::endl;
    std::cout << nx::Int<nx::Shift<int, 7, -3>()>::value << std::endl;
    std::cout << nx::Int<nx::Shift<int, 7>(-3)>::value << std::endl;

    std::cout << nx::Int<nx::Shift<0>(7)>::value << std::endl;
    std::cout << nx::Int<nx::Shift<0, int>(7)>::value << std::endl;
    std::cout << nx::Int<nx::Shift<int, 7, 0>()>::value << std::endl;
    std::cout << nx::Int<nx::Shift<int, 7>(0)>::value << std::endl;

    std::cout << nx::Int<nx::Shift<3>(7)>::value << std::endl;
    std::cout << nx::Int<nx::Shift<3, int>(7)>::value << std::endl;
    std::cout << nx::Int<nx::Shift<int, 7, 3>()>::value << std::endl;
    std::cout << nx::Int<nx::Shift<int, 7>(3)>::value << std::endl;

    return 0;
  }
};
/// @brief Function to lazy-load the application; required by nx_main.cc
nx::Application& nx::GetApplication() {
  static SandboxApplication app;
  return app;
}
