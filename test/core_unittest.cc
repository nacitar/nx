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

/// @file core_unittest.cc
/// @brief Unit tests for core.h
/// @todo DependentConstant, DependentBool, EnableIf/DisableIf, InvalidType(?)
/// @todo Move "EXPECT_TYPE" somewhere more common.

#include <type_traits>
#include "gtest/gtest.h"
#include "nx/core.h"

#define EXPECT_TYPE(x, y) EXPECT_TRUE((std::is_same<x, y>::value))

// Checks that the values and types for a the given integral_constant derived
// class are as expected.
#define INTEGRAL_TEST(Type, Real, kValue1, kValue2) {\
  typedef typename Type<kValue1>::value_type value_type; \
  typedef decltype(Type<kValue1>::value) type; \
  typedef typename Type<kValue2>::value_type value_type2; \
  typedef decltype(Type<kValue2>::value) type2; \
  EXPECT_EQ(Type<kValue1>::value, kValue1); \
  EXPECT_EQ(Type<kValue2>::value, kValue2); \
  EXPECT_TYPE(value_type, Real); \
  EXPECT_TYPE(value_type2, Real); \
  EXPECT_TYPE(type, const Real); \
  EXPECT_TYPE(type2, const Real); \
}

TEST(CoreTest, Bool) {
  INTEGRAL_TEST(nx::Bool, bool, true, false);
}

TEST(CoreTest, Int) {
  INTEGRAL_TEST(nx::Int, int, 1234, -1234);
}

TEST(CoreTest, UInt) {
  INTEGRAL_TEST(nx::UInt, unsigned int, 1234u, 54321u);
}

TEST(CoreTest, True) {
  typedef decltype(nx::True::value) type;
  EXPECT_TRUE(nx::True::value);
  EXPECT_TYPE(nx::True::value_type, bool);
  EXPECT_TYPE(type, const bool);
}

TEST(CoreTest, False) {
  typedef decltype(nx::False::value) type;
  EXPECT_FALSE(nx::False::value);
  EXPECT_TYPE(nx::False::value_type, bool);
  EXPECT_TYPE(type, const bool);
}

TEST(CoreTest, Identity) {
  EXPECT_TYPE(nx::Identity<int>::type, int);
}

TEST(CoreTest, Invoke) {
  EXPECT_TYPE(nx::Invoke<nx::Identity<int>>, int);
}

TEST(CoreTest, Conditional) {
  typedef nx::Conditional<nx::True, int, void> if_type;
  typedef nx::Conditional<nx::False, int, void> else_type;
  EXPECT_TYPE(if_type, int);
  EXPECT_TYPE(else_type, void);
}

TEST(CoreTest, Not) {
  EXPECT_FALSE(nx::Not<nx::True>::value);
  EXPECT_TRUE(nx::Not<nx::False>::value);
  EXPECT_FALSE(nx::Not<nx::UInt<1234>>::value);
  EXPECT_TRUE(nx::Not<nx::UInt<0>>::value);
}

TEST(CoreTest, Any) {
  // Integral values are handled properly
  EXPECT_TRUE(nx::Any<nx::UInt<1234>>::value);
  EXPECT_FALSE(nx::Any<nx::UInt<0>>::value);
  // Mixed-values are handled properly, and further integral checks
  EXPECT_TRUE((nx::Any<nx::False, nx::UInt<1234>, nx::False>::value));
  EXPECT_FALSE((nx::Any<nx::False, nx::UInt<0>, nx::False>::value));
  // Test general functionality
  // Permutations of 1
  EXPECT_TRUE(nx::Any<nx::True>::value);
  EXPECT_FALSE(nx::Any<nx::False>::value);
  // Permutations of 2
  EXPECT_FALSE((nx::Any<nx::False, nx::False>::value));
  EXPECT_TRUE((nx::Any<nx::False, nx::True>::value));
  EXPECT_TRUE((nx::Any<nx::True, nx::False>::value));
  EXPECT_TRUE((nx::Any<nx::True, nx::True>::value));
  // Permutations of 3
  EXPECT_FALSE((nx::Any<nx::False, nx::False, nx::False>::value));
  EXPECT_TRUE((nx::Any<nx::False, nx::False, nx::True>::value));
  EXPECT_TRUE((nx::Any<nx::False, nx::True, nx::False>::value));
  EXPECT_TRUE((nx::Any<nx::False, nx::True, nx::True>::value));
  EXPECT_TRUE((nx::Any<nx::True, nx::False, nx::False>::value));
  EXPECT_TRUE((nx::Any<nx::True, nx::False, nx::True>::value));
  EXPECT_TRUE((nx::Any<nx::True, nx::True, nx::False>::value));
  EXPECT_TRUE((nx::Any<nx::True, nx::True, nx::True>::value));
}

TEST(CoreTest, All) {
  // Integral values are handled properly
  EXPECT_TRUE(nx::All<nx::UInt<1234>>::value);
  EXPECT_FALSE(nx::All<nx::UInt<0>>::value);
  // Mixed-values are handled properly, and further integral checks
  EXPECT_TRUE((nx::All<nx::True, nx::UInt<1234>, nx::True>::value));
  EXPECT_FALSE((nx::All<nx::True, nx::UInt<0>, nx::True>::value));
  // Test general functionality
  // Permutations of 1
  EXPECT_TRUE(nx::All<nx::True>::value);
  EXPECT_FALSE(nx::All<nx::False>::value);
  // Permutations of 2
  EXPECT_FALSE((nx::All<nx::False, nx::False>::value));
  EXPECT_FALSE((nx::All<nx::False, nx::True>::value));
  EXPECT_FALSE((nx::All<nx::True, nx::False>::value));
  EXPECT_TRUE((nx::All<nx::True, nx::True>::value));
  // Permutations of 3
  EXPECT_FALSE((nx::All<nx::False, nx::False, nx::False>::value));
  EXPECT_FALSE((nx::All<nx::False, nx::False, nx::True>::value));
  EXPECT_FALSE((nx::All<nx::False, nx::True, nx::False>::value));
  EXPECT_FALSE((nx::All<nx::False, nx::True, nx::True>::value));
  EXPECT_FALSE((nx::All<nx::True, nx::False, nx::False>::value));
  EXPECT_FALSE((nx::All<nx::True, nx::False, nx::True>::value));
  EXPECT_FALSE((nx::All<nx::True, nx::True, nx::False>::value));
  EXPECT_TRUE((nx::All<nx::True, nx::True, nx::True>::value));
}


TEST(CoreTest, CheckValidType) {
  typedef nx::CheckValidType<false, int, float>::type if_type;
  // shouldn't assert
  typedef nx::CheckValidType<true, int, float>::type if_type_assert;
  typedef nx::CheckValidType<false, nx::InvalidType, void>::type else_type;
  EXPECT_TYPE(if_type, int);
  EXPECT_TYPE(if_type_assert, int);
  EXPECT_TYPE(else_type, void);
  // NOTE: not checking the assertion failure
}

TEST(CoreTest, AssertValidType) {
  typedef nx::AssertValidType<int, float>::type if_type_assert;
  EXPECT_TYPE(if_type_assert, int);
  // NOTE: not checking the assertion falure
}

TEST(CoreTest, IsValidType) {
  typedef nx::IsValidType<int, float>::type if_type;
  typedef nx::IsValidType<nx::InvalidType, void>::type else_type;
  EXPECT_TYPE(if_type, int);
  EXPECT_TYPE(else_type, void);
  // NOTE: not checking the assertion falure
}

TEST(CoreTest, BitSize) {
  EXPECT_EQ(nx::Bits<int>::Size(),
      sizeof(int)*CHAR_BIT);
  EXPECT_EQ(nx::Bits<float>::Size(),
      sizeof(float)*CHAR_BIT);
  EXPECT_EQ(nx::Bits<char>::Size(),
      sizeof(char)*CHAR_BIT);
}

TEST(CoreTest, BitRange) {
  constexpr size_t bits = nx::Bits<int>::Size();
  EXPECT_TRUE((nx::Bits<int>::InRange<bits, bits>()));
  EXPECT_FALSE((nx::Bits<int>::InRange<bits+2, bits+5>()));
  EXPECT_FALSE((nx::Bits<int>::InRange<bits+1, bits+4>()));
  EXPECT_TRUE((nx::Bits<int>::InRange<bits, bits+3>()));
  EXPECT_TRUE((nx::Bits<int>::InRange<bits-2, bits+2>()));
  EXPECT_FALSE((nx::Bits<int>::InRange<bits-1, bits-1>()));
}

TEST(CoreTest, SetSigned) {
  typedef nx::SetSigned<true, int>::type signed_to_signed;
  typedef nx::SetSigned<false, int>::type signed_to_unsigned;
  typedef nx::SetSigned<true, unsigned int>::type unsigned_to_signed;
  typedef nx::SetSigned<false, unsigned int>::type unsigned_to_unsigned;
  typedef nx::SetSigned<false, signed char>::type char_signed_to_unsigned;
  EXPECT_TYPE(signed_to_signed, int);
  EXPECT_TYPE(signed_to_unsigned, unsigned int);
  EXPECT_TYPE(unsigned_to_signed, int);
  EXPECT_TYPE(unsigned_to_unsigned, unsigned int);
  EXPECT_TYPE(char_signed_to_unsigned, unsigned char);
}

TEST(CoreTest, MultiplicationOverflow) {
  constexpr unsigned char high_bit =
      static_cast<unsigned char>(~(static_cast<unsigned char>(~0)>>1));
  EXPECT_EQ(high_bit, 0x80);
  // Oveflow by 1
  EXPECT_TRUE((nx::Bits<unsigned char>::MultiplicationOverflow<high_bit, 2>()));
  EXPECT_FALSE((nx::Bits<unsigned char>::MultiplicationOverflow<5, 5>()));
}

TEST(CoreTest, Power) {
  constexpr unsigned char high_bit =
      static_cast<unsigned char>(~(static_cast<unsigned char>(~0)>>1));
  constexpr size_t bitsize = nx::Bits<unsigned char>::Size();
  EXPECT_EQ((nx::Bits<int>::Power<9, 0>()), 1);
  EXPECT_EQ((nx::Bits<int>::Power<9, 1>()), 9);
  EXPECT_EQ((nx::Bits<int>::Power<9, 2>()), 81);
  EXPECT_EQ((nx::Bits<unsigned char>::Power<2, bitsize-1>()), high_bit);
}
