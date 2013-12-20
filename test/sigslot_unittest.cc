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

/// @file digits_unittest.cc
/// @brief Unit tests for digits.h
/// @todo These tests are very incomplete.

#include "gtest/gtest.h"
#include "nx/sigslot.h"

class SimpleSlotObject {
  nx::SlotRegistrar registrar_;
 public:
    int fireCount;
    static int mooseCount;
    explicit SimpleSlotObject(nx::Signal<int>* signal)
        : fireCount(0) {
      signal->connect(&registrar_, this, &SimpleSlotObject::onFire);
      signal->connect(&registrar_, this, &SimpleSlotObject::onFire);
      signal->connect(&registrar_, onMoose);
    }

    void onFire(int x) {
      ++fireCount;
    }

    static void onMoose(int x) {
      ++mooseCount;
    }
};
int SimpleSlotObject::mooseCount = 0;


TEST(SigSlotTest, Basic) {
  nx::Signal<int> realSignal;
  SimpleSlotObject obj(&realSignal);

  nx::SlotRegistrar registrar;
  nx::Signal<int> otherSignal;
  otherSignal.connect(&registrar, &realSignal);

  otherSignal.emit(5);

  EXPECT_EQ(obj.fireCount, 2);
  EXPECT_EQ(SimpleSlotObject::mooseCount, 1);
}
