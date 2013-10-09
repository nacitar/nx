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

#include <type_traits>
#include "gtest/gtest.h"
#include "nx/core.h"

// TODO: put in a place for all tests
#define EXPECT_TYPE(x,y) EXPECT_TRUE((std::is_same<x,y>::value))

// Checks that the values and types for a the given integral_constant derived
// class are as expected.
#define INTEGRAL_TEST(Type,Real,kValue1,kValue2) {\
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

TEST(CoreTest, Identity) {
  EXPECT_TYPE(nx::Identity<int>::type, int);
}

TEST(CoreTest, Invoke) {
  EXPECT_TYPE(nx::Invoke<nx::Identity<int>>, int);
}
