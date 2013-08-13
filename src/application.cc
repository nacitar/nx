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

/// @file application.cc
/// @brief Implementation for application.h

#include "nx/application.h"

/// @brief Library namespace.
namespace nx {

Application::~Application() {
}

Application::Application() {
}

bool Application::set_arguments(
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

const PlatformData* Application::platform_data() const {
  return platform_data_.get();
}
PlatformData* Application::platform_data() {
  return platform_data_.get();
}
void Application::set_platform_data(PlatformData*data) {
  platform_data_.reset(data);
}

}  // namespace nx

/// @brief Main driver; invokes the {@link nx::Application} instance for this
/// app.
int main(int argc, char*argv[]) {
  nx::PlatformData * data = new nx::PlatformData();

  nx::Application&application = nx::GetApplication();
  application.set_platform_data(data);
  application.set_arguments(argc, argv);
  return application.main();
}

#ifdef NX_OS_WINDOWS
#include <stdlib.h>  // for __argc and __argv
/// @brief Windows main driver; invokes the {@link nx::Application} instance
/// for this app.
int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow) {
  nx::PlatformData * data = new nx::PlatformData();
  data->hInstance = hInstance;
  data->nCmdShow = nCmdShow;

  nx::Application&application = nx::GetApplication();
  application.set_platform_data(data);
  // TODO(nacitar): make this utf8 from GetCommandLine()
  application.set_arguments(__argc, __argv);
  return application.main();
}
#endif

