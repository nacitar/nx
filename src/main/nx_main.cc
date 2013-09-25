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

/// @file nx_main.cc
/// @brief An implementation of the main()/other startup functions required
/// to properly instantiate the user's application instance and run its main.

#include "nx/application.h"

/// @brief Main driver; invokes the {@link nx::Application} instance for this
/// app.
int main(int argc, char*argv[]) {
  nx::PlatformData * data = new nx::PlatformData();
  data->argc=argc;
  data->argv=argv;

  nx::Application&application = nx::GetApplication();
  application.set_platform_data(data);
  application.set_arguments(data->argc, data->argv);
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
  // TODO(nacitar): make this utf8 from GetCommandLine()
  data->argc=__argc;
  data->argv=__argv;
  data->hInstance = hInstance;
  data->nCmdShow = nCmdShow;

  nx::Application&application = nx::GetApplication();
  application.set_platform_data(data);
  application.set_arguments(data->argc, data->argv);
  return application.main();
}
#endif  // NX_OS_WINDOWS
