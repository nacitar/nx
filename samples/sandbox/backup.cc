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

/// @file main.cc
/// @brief A place for development.

#include <iostream>
#include "nx/core.h"
#include "nx/application.h"

namespace nx {


NX_FORCEINLINE constexpr size_t sequence() {
  return 0;
}
template <size_t kFirst>
NX_FORCEINLINE constexpr size_t sequence() {
  return 1;
}
template <size_t kFirst, size_t kNext, size_t ... Rest>
NX_FORCEINLINE constexpr size_t sequence() {
  return ((kFirst+1u) == kNext) ? 1 + sequence<kNext, Rest...>() : 1;
}



template <ssize_t kOffset, class T>
NX_FORCEINLINE constexpr EnableIf<Bool<
    (kOffset == 0)>,
    T> Shift(T value) {
  return value;
}
template <ssize_t kOffset, class T>
NX_FORCEINLINE constexpr EnableIf<Bool<
    (kOffset < 0 && kOffset > -static_cast<ssize_t>(Bits<T>::Size()))>,
    T> Shift(T value) {
  return (value << -kOffset);
}
template <ssize_t kOffset, class T>
NX_FORCEINLINE constexpr EnableIf<Bool<
    (kOffset > 0 && kOffset < static_cast<ssize_t>(Bits<T>::Size()))>,
    T> Shift(T value) {
  return (value >> kOffset);
}
template <ssize_t kOffset, class T>
NX_FORCEINLINE constexpr EnableIf<Bool<(
    kOffset <= -static_cast<ssize_t>(Bits<T>::Size()) ||
    kOffset >= static_cast<ssize_t>(Bits<T>::Size()))>,
    T> Shift(T value) {
  // shifting >= sizeof type, just zeroing it out
  // would be UB if we tried it anyway.
  return 0;
}


template <class T, unsigned int skip_, unsigned int offset_, int delta_>
NX_FORCEINLINE constexpr T getMask() {
  return static_cast<T>(0);
}
// Forward Declaration
template <class T, unsigned int skip_, unsigned int offset_,
    int delta_, unsigned int index_, unsigned int ... rest_>
NX_FORCEINLINE constexpr EnableIf<Bool<
    skip_ != 0>,
    T> getMask();
template <class T, unsigned int skip_, unsigned int offset_,
    int delta_, unsigned int index_, unsigned int ... rest_>
NX_FORCEINLINE constexpr EnableIf<Bool<
    skip_ == 0 && static_cast<int>(offset_ - index_) == delta_>,
    T> getMask() {
  typedef UInt<sequence<index_, rest_...>()> seq;
  return Shift<-offset_>(Bits<T>::template LowMask<seq::value>())
      | getMask<T, seq::value - 1, offset_ + seq::value, delta_, rest_...>();
}
template <class T, unsigned int skip_, unsigned int offset_,
    int delta_, unsigned int index_, unsigned int ... rest_>
NX_FORCEINLINE constexpr EnableIf<Bool<
    skip_ == 0 && static_cast<int>(offset_ - index_) != delta_>,
    T> getMask() {
  typedef UInt<sequence<index_, rest_...>()> seq;
  return getMask<T, seq::value - 1, offset_ + seq::value, delta_, rest_...>();
}
template <class T, unsigned int skip_, unsigned int offset_,
    int delta_, unsigned int index_, unsigned int ... rest_>
NX_FORCEINLINE constexpr EnableIf<Bool<
    skip_ != 0>,
    T> getMask() {
  return getMask<T, skip_-1, offset_, delta_, rest_...>();
}





template <class T, T ... values_>
class Pack {
  template <size_t index_, T value_, T ... rest_>
  static NX_FORCEINLINE constexpr EnableIf<Bool<
      (index_ == 0u)>,
      T> getParameterValue() {
    return value_;
  }
  template <size_t index_, T value_, T ... rest_>
  static NX_FORCEINLINE constexpr EnableIf<Bool<(index_ != 0u)>,
      T> getParameterValue() {
    return getParameterValue<index_-1, rest_...>();
  }
 public:
  typedef T type;

  static NX_FORCEINLINE constexpr bool validIndex(size_t index) {
    return (index < sizeof...(values_));
  }
  static NX_FORCEINLINE constexpr bool lastValidIndex(size_t index) {
    return !empty() && index == size()-1;
  }
  static NX_FORCEINLINE constexpr size_t size() {
    return sizeof...(values_);
  }
  static NX_FORCEINLINE constexpr bool empty() {
    return size() == 0;
  }
  template <size_t index_>
  static NX_FORCEINLINE constexpr EnableIf<Bool<
      (index_ < sizeof...(values_))>,
      T> get() {
    return getParameterValue<index_, values_...>();
  }
  template <size_t index_=0>
  static NX_FORCEINLINE constexpr EnableIf<All<
      std::is_integral<T>, Bool<validIndex(index_+1)>>,
      size_t> sequence() {
        return ((get<index_>()+1 == get<index_+1>())
            ? 1 + sequence<index_+1>()
            : 1);
  }
  template <size_t index_=0>
  static NX_FORCEINLINE constexpr EnableIf<All<
      std::is_integral<T>, Bool<lastValidIndex(index_)>>,
      size_t> sequence() {
        return 1;
  }
  template <size_t index_=0>
  static NX_FORCEINLINE constexpr EnableIf<All<
      std::is_integral<T>, Bool<!validIndex(index_)>>,
      size_t> sequence() {
        return 0;
  }
};

template <size_t ... values_>
using IndexPack = Pack<size_t, values_...>;



template <class T, class PackedIndexes_, size_t i=0, size_t offset_=0>
NX_FORCEINLINE constexpr EnableIf<Bool<
    (!PackedIndexes_::validIndex(i))>,
    T> toField(T value) {
  return 0;
}

template <class T, class PackedIndexes_, size_t i=0, size_t offset_=0>
NX_FORCEINLINE constexpr EnableIf<Bool<
    (PackedIndexes_::validIndex(i))>,
    T> toField(T value) {
  typedef Size<PackedIndexes_::template sequence<>()> seq;
  typedef Constant<T,
      Shift<-offset_>(~(~static_cast<T>(0) << seq::value))> mask_;

  return Shift<-PackedIndexes_::template get<i>() + offset_>(value & mask_::value) |
      toField<T, PackedIndexes_, i+seq::value, offset_+seq::value>(value);
}

}  // namespace nx
template <class T, size_t ... indexes>
NX_FORCEINLINE constexpr T toField(T value) {
  return nx::toField<T, nx::IndexPack<indexes...>>(value);
}
/// @brief The class for the Sandbox application.
class SandboxApplication : public nx::Application {
 public:
  int main() {
    typedef nx::IndexPack<4, 5, 3> MyArray;
    constexpr unsigned int value = 0b001;
    std::cout << MyArray::get<1>() << std::endl;
    std::cout << nx::toField<unsigned int, MyArray>(value) << std::endl;
    std::cout << toField<unsigned int, 4, 5, 3>(value) << std::endl;
    std::cout << nx::getMask<unsigned int, 0, 0, -2, 2, 3, 0, 1, 6, 7, 4, 5>() << std::endl;

#if 0
    constexpr const unsigned int value = 0b1000;
    std::cout << nx::UInt<BU::ScanForward<value>()>() << std::endl;
    std::cout << nx::UInt<BU::ScanReverse<value>()>() << std::endl;
    std::cout << nx::UInt<BU::PopCount<value>()>() << std::endl;
    std::cout << nx::UInt<BU::Size()>() << std::endl;
    std::cout << BU::MultiplicationOverflow(value,value) << std::endl;
    std::cout << nx::Bool<BU::MultiplicationOverflow<value,value>()>() << std::endl;
    std::cout << std::endl;
    std::cout << BU::PowerOfTwo(value) << std::endl;
    std::cout << nx::Bool<BU::PowerOfTwo<value>()>() << std::endl;
    std::cout << std::endl;
    std::cout << BU::Mask(1,3,5) << std::endl;
    std::cout << nx::UInt<BU::Mask<1,3,5>()>() << std::endl;

    unsigned int dest = value + 16;
    std::cout << "orignl: " << dest << std::endl;
    BU::assign(value,0,&dest);
    std::cout << "assign: " << dest << std::endl;
    BU::set(value,&dest);
    std::cout << "set: " << dest << std::endl;
    BU::assign<value>(0,&dest);
    std::cout << "assign: " << dest << std::endl;
    BU::set<value>(&dest);
    std::cout << "set: " << dest << std::endl;
    BU::assign<value,0>(&dest);
    std::cout << "assign: " << dest << std::endl;
    BU::clear(value,&dest);
    std::cout << "clear: " << dest << std::endl;
    dest=value+16;
    BU::clear<value>(&dest);
    std::cout << "clear: " << dest << std::endl;

    std::cout << std::endl;

    std::cout << nx::UInt<BU::Power<5, 2>()>() << std::endl;
#endif
    return 0;
  }
};

/// @brief Function to lazy-load the application; required by nx_main.cc
nx::Application& nx::GetApplication() {
  static SandboxApplication app;
  return app;
}
