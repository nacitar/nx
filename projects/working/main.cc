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

#include <stdio.h>
#include <iostream>
#include <sstream>
#include "nx/application.h"
#include "nx/time.h"
#include "nx/constant.h"
#include "nx/bitscanreverse.h"
#include "nx/bitscanforward.h"
#include "nx/tostring.h"
#include "nx/popcount.h"
#include "nx/reverse.h"
#include "nx/digits.h"


namespace nx {
  namespace detail {
    template<char... digits>
    struct binary_literal_helper;

    template<char high, char... digits>
    struct binary_literal_helper<high, digits...> {
      // +1 to include 'high'
      typedef uint_least_t<sizeof...(digits)+1> uint_type;
      static_assert(high == '0' || high == '1', "invalid binary digit!");
      static uint_type const value =
        (static_cast<uint_type>(high - '0') << sizeof...(digits))
        + binary_literal_helper<digits...>::value;
    };

    template<char high>
    struct binary_literal_helper<high> {
      typedef uint_least_t<1> uint_type;
      static_assert(high == '0' || high == '1', "invalid binary digit!");
      static uint_type const value = (high - '0');
    };
  }  // namespace detail
}  // namespace nx


template<char... digits>
constexpr nx::uint_least_t<sizeof...(digits)> operator "" _b() {
  return nx::detail::binary_literal_helper<digits...>::value;
}

template <unsigned int base, unsigned int power>
using IntPow = nx::constant::Power<unsigned int,base,power>;
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
  void test_bitops(nx::uint_least64_t x) {
    std::cout << "BSF: " << nx::BitScanForward(x) << std::endl;
    std::cout << "BSR: " << nx::BitScanReverse(x) << std::endl;
    std::cout << "dig: " << nx::Digits(x) << std::endl;
  }
  int test_strings() {
    //char buf[100];
    int sum=0;
    std::string buf;
    for (int i=0;i<100000000;++i) {
      sum += nx::ToString(-i,&buf);
      buf.clear();
    }
    return sum;
  }


  int main() {
    arg_vector& args = arguments();
    if (args.size() != 2) {
      std::cerr << "INVALID" << std::endl;
      return 1;
    }
    auto blit_val = 11110000_b;
    auto pow_val = IntPow<10,9>::value;
    nx::int_least64_t cmdline_val;
    {
      std::stringstream ss(args[1]);
      ss >> cmdline_val;
    }
    char buf[256];
    std::cout << "blit_val reverse: " << +nx::Reverse(blit_val) << std::endl;
    std::cout << "cmdline_val: " << cmdline_val << std::endl;
    std::cout << "power test: " << pow_val << std::endl;
    std::cout << "digits_test: " << nx::Digits(cmdline_val) << std::endl;
    std::cout << "tos test: " << nx::ToString(cmdline_val) << std::endl;
    std::cout << "tos test pad: " << nx::ToString(cmdline_val,10) << std::endl;
    int len=nx::ToString(-1234,buf);
    buf[len]='\0';
    std::cout << "buf: " << buf << std::endl;


    return 0;
  }
};

// @brief Function to lazy-load the application; required by nx library.
nx::Application& nx::get_application() {
  static MyApplication app;
  return app;
}
