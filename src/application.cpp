/*
 * Copyright (C) 2013 Jacob McIntosh <nacitar at ubercpp dot com> 
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*! @file
 *  Implementation for application.hpp 
 */ 

#include <nx/application.hpp>

namespace nx
{
  Application::~Application() {
  }

  Application::Application() {
  }

  Application::Application(const int argc,const char* const * const argv) {
    set_arguments(argc,argv);
  }
  
  bool Application::set_arguments(
      const int argc,
      const char* const * const argv) {
    if (argc < 0) return false;
    arguments_.assign(argv,argv+argc);
    return true;
  }

  const Application::arg_vector& Application::arguments() const {
    return arguments_;
  }
  Application::arg_vector& Application::arguments() {
    return arguments_;
  }

  const Application::PlatformData* Application::platform_data() const {
    return platform_data_.get();
  }
  Application::PlatformData* Application::platform_data() {
    return platform_data_.get();
  }
  void Application::set_platform_data(Application::PlatformData*data) {
    platform_data_.reset(data);
  }

}

//! Main driver; invokes the Application instance for this app.
int main(int argc, char*argv[]) {
	nx::Application::PlatformData * data = new nx::Application::PlatformData();
  nx::application.set_platform_data(data);
  nx::application.set_arguments(argc,argv);
  return nx::application.main();
}

#ifdef NX_OS_WINDOWS
#include <stdlib.h> // for __argc and __argv
//! Windows main driver; invokes the Application instance for this app.
int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,int nCmdShow) {
	nx::Application::PlatformData * data = new nx::Application::PlatformData();
	data->hInstance=hInstance;
	data->nCmdShow=nCmdShow;

  nx::application.set_platform_data(data);
  // TODO: make this utf8 from GetCommandLine()
  nx::application.set_arguments(__argc,__argv);
  return nx::application.main();
}
#endif

