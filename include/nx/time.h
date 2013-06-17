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
/// A collection of functions to do with time or timing.

#ifndef INCLUDE_NX_TIME_H_
#define INCLUDE_NX_TIME_H_

#include "nx/core.h"

#ifndef NX_OS_WINDOWS
  // symbols telling me if clock_gettime/monotonic timers are available
  #include <time.h>
  #include <unistd.h>
  #if _POSIX_TIMERS <= 0 || !defined(_POSIX_MONOTONIC_CLOCK) || \
      !defined(CLOCK_MONOTONIC_RAW)
    #error The timer library requires POSIX CLOCK_MONOTONIC_RAW support.
  #endif
#endif
namespace nx {
  #ifndef NX_OS_WINDOWS
  // Sized to match the "seconds" field in timespec
  typedef uint_t<BitSize<time_t>::value> msec_t;
  #else
  // Matches GetTickCount64's return value
  typedef uint_t<BitSize<ULONGLONG>::value> msec_t;
  #endif
  // Forward to the correct specialization
  msec_t fixed_msec();
  msec_t msec_elapsed(msec_t later, msec_t earlier);

  void millisleep(const unsigned int millisecs);
}  // namespace nx

#endif  // INCLUDE_NX_TIME_H_
