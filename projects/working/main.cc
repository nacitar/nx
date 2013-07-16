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
/// Test project for things being worked on.

#include <iostream>
#include <sstream>
#include <stdio.h>
#include "nx/log10.h"
#include "nx/application.h"
#include "nx/time.h"
#include "nx/constant.h"
#include "nx/bitscanreverse.h"
#include "nx/bitscanforward.h"

/// The class for the test application
class MyApplication : public nx::Application {
 public:
  void timer_resolution_test() {
    nx::msec_t last = 0;
    while (true) {
      nx::msec_t now = nx::fixed_msec();
      if (last != now) {
        printf("%d", static_cast<unsigned int>(now));
        last = now;
      }
    }
  }
  int main() {
    arg_vector&args=arguments();
    if (args.size() != 2) {
      std::cerr << "INVALID" << std::endl;
      return 1;
    }
    nx::uint_least64_t x;
    std::stringstream ss(args[1]);
    ss >> x;
    std::cout << "BSF: " << nx::bitScanForward(x) << std::endl;
    std::cout << "BSR: " << nx::bitScanReverse(x) << std::endl;
    std::cout << "L10: " << nx::log10(x) << std::endl; 
    return 0;
  }
};

// Set this as the default application
nx::Application& nx::application = *new MyApplication();
