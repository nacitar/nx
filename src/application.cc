//
// Copyright (C) 2014 Jacob McIntosh <nacitar at ubercpp dot com>
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

/// @file application.cc
/// @brief Implementation for application.h

#include "nx/application.h"
#include "nx/looper.h"

/// @brief Library namespace.
namespace nx {

Application::~Application() {
}

Application::Application() {
}

bool Application::setArguments(
    const int argc,
    const char* const * const argv) {
  if (argc < 0) return false;
  arguments_.assign(argv, argv+argc);
  return true;
}

const Application::ArgumentVector& Application::arguments() const {
  return arguments_;
}
Application::ArgumentVector& Application::arguments() {
  return arguments_;
}

const PlatformData* Application::platformData() const {
  return platformData_.get();
}
PlatformData* Application::platformData() {
  return platformData_.get();
}
void Application::setPlatformData(PlatformData*data) {
  platformData_.reset(data);
}

namespace detail {
const std::thread::id mainThreadId = std::this_thread::get_id();
}  // namespace detail

std::thread::id MainThreadId() {
  return detail::mainThreadId;
}

void Initialize() {
  static bool initialized = false;
  if (initialized || std::this_thread::get_id() != MainThreadId()) {
    return;
  }
  initialized = true;
  Looper::prepare();
}

}  // namespace nx
