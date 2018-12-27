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

/// @file application.h
/// @brief Tools to allow for an object-oriented application approach.

#ifndef INCLUDE_NX_APPLICATION_H_
#define INCLUDE_NX_APPLICATION_H_

#include <memory>
#include <vector>
#include <string>
#include <thread>

#include "nx/core.h"
#include "nx/thread_compat.h"

/// @brief Library namespace.
namespace nx {

/// @brief A class used to hold platform-specific data that is sometimes
/// pertinent that would typically be obtained in a traditional main()
/// function.
#ifdef NX_TARGET_WINDOWS
struct PlatformData {
  // These members are named matching winapi documentation.

  /// @brief The hInstance of the application, as passed to WinMain()
  HINSTANCE hInstance;
  /// @brief The value of nCmdShow as passed to WinMain()
  int nCmdShow;
  /// @brief The number of arguments passed on the command line.
  int argc;
  /// @brief The array of arguments passed on th ecommand line.
  char**argv;
};
#else
struct PlatformData {
  /// @brief The number of arguments passed on the command line.
  int argc;
  /// @brief The array of arguments passed on th ecommand line.
  char**argv;
};
#endif

/// @brief A base class upon which an application can be built.
class Application {
 public:
  /// @brief The container type used to hold arguments.
  typedef std::vector<std::string> ArgumentVector;

  /// @brief Destructor.
  virtual ~Application();

  /// @brief Constructs with no stored arguments.
  Application();

  /// @brief The driver function of the application.  Must be overridden.
  ///
  /// @return The exit code of the application.
  virtual int main() = 0;

  /// @brief Populates the stored arguments with the ones provided.
  ///
  /// @param argc The number of arguments passed.
  /// @param argv The array of argument values.
  /// @return true if the arguments were set successfully.
  bool setArguments(const int argc, const char* const * const argv);

  /// @brief Accessor for the stored arguments.
  /// @return A const reference to the stored arguments.
  const ArgumentVector& arguments() const;

  /// @brief Accessor for the stored arguments.
  /// @return A reference to the stored arguments.
  ArgumentVector& arguments();

  /// @brief Accessor for any platform-specific data.
  /// @return A const pointer to the PlatformData.
  const PlatformData* platformData() const;

  /// @brief Accessor for any platform-specific data.
  /// @return A pointer to the PlatformData.
  PlatformData* platformData();

  /// @brief Sets the platform-specific data, transferring ownership to this
  /// object.
  ///
  /// @param data The new platform-specific data.  Must be allocated with new.
  void setPlatformData(PlatformData*data);

 private:
  /// @brief The platform-specific data.
  std::unique_ptr<PlatformData> platformData_;

  /// @brief The stored arguments.
  ArgumentVector arguments_;
};

/// @brief The main application.  This is _not_ implemented by this library;
/// the user is expected to link in an implementation for use.  However, this
/// is only necessary if the user builds/links with the provided nx_main.cc in
/// order to instantiate the application, because no other portion of the
/// library references this function.
Application& GetApplication();

namespace detail {
  extern const std::thread::id mainThreadId;
}  // namespace detail

/// @brief Returns the id of the thread which invoked the application's entry
/// point.
std::thread::id MainThreadId();

/// @brief Performs common initialization; must be called from the main thread.
void Initialize();

}  // namespace nx

#endif  // INCLUDE_NX_APPLICATION_H_
