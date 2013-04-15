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
 *  Tools to allow for an object-oriented application approach.
 */ 

#ifndef NX_APPLICATION_HPP_
#define NX_APPLICATION_HPP_

#include <nx/core.hpp>

#include <memory>
#include <vector>
#include <string>

namespace nx
{
  //! A base class upon which an application can be built.
  class Application
  {
   public:
    //! The container type used to hold arguments.
    typedef std::vector<std::string> arg_vector;

    #ifdef NX_OS_WINDOWS
    //! A class used to hold platform-specific data that is sometimes pertinent
    //! that would typically be obtained in a tradtional main() function.
    struct PlatformData {
      HINSTANCE hInstance;
      int nCmdShow;
    };
    #else
    struct PlatformData {
    };
    #endif

    virtual ~Application();

    Application();

    //! Constructs the application using the provided arguments.
    Application(const int argc,const char* const * const argv);
    
    //! The driver function of the application.  This should be overridden in
    //! a child class.
    virtual int main()=0;
    
    //! Populates the arguments using the argc/argv pair passed.
    bool set_arguments(const int argc,const char* const * const argv);

    const arg_vector& arguments() const;
    arg_vector& arguments();

    const PlatformData* platform_data() const;
    PlatformData* platform_data();
    void set_platform_data(PlatformData*data);

   private:
    std::unique_ptr<PlatformData> platform_data_;
    arg_vector arguments_;
  };
  

  //! The main application;  defined as an external to ensure a linker error if
  //! the user does not instantiate an application.
  extern Application& application;

}

#endif  // NX_APPLICATION_HPP_
