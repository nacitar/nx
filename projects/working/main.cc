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
#include "nx/application.h"
#include "nx/time.h"


/// The class for the test application
struct MyApplication : nx::Application {
  int main() {
    nx::msec_t last = 0;
    while (true) {
      nx::msec_t now = nx::fixed_msec();
      if (last != now) {
        printf("%d", static_cast<unsigned int>(now));
        last = now;
      }
    }
    return 0;
  }
};

// Set this as the default application
nx::Application& nx::application = *new MyApplication();
