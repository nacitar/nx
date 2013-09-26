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
/// @brief Sample project showing the nx model for a very simple hello world
/// application.

#include <iostream>
#include "nx/core.h"
#include "nx/application.h"

/// @brief The class for the Hello World application.
class HelloWorldApplication : public nx::Application {
 public:
  int main() {
    auto&args = arguments();
    std::cout << "Hello World!" << std::endl;
    return 0;
    return 0;
  }
};

/// @brief Function to lazy-load the application; required by nx_main.cc
nx::Application& nx::GetApplication() {
  static HelloWorldApplication app;
  return app;
}
