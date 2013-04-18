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
 *  The implementation for time.hpp 
 */ 

#include <nx/time.hpp>

#include <stdexcept>

// NOTE: windows precision won't be 1ms precision; we aren't overriding the
// system clock interval (as it is bad for the system to do that anyway; uses
// battery a lot more).  Expect 12-16ms precision

namespace nx
{
  #ifndef NX_OS_WINDOWS
  msec_t fixed_msec() { 
    struct timespec time;
    if(NX_UNLIKELY(clock_gettime(CLOCK_MONOTONIC_RAW, &time) != 0)) {
      throw std::runtime_error(
          "Failed to retrieve value from monotonic clock!");
    }
    return (
        (static_cast<msec_t>(time.tv_sec) * 1000) +
        (time.tv_nsec / 1000000));
  }
  msec_t msec_elapsed(msec_t later, msec_t earlier) {
    return (later-earlier);
  }
  void millisleep(const unsigned int millisecs) {
    struct timespec tspec;
    // Get the number of seconds
    tspec.tv_sec = millisecs/1000ul;
    // Subtract the seconds off of the milliseconds, and convert to nanoseconds
    tspec.tv_nsec = (millisecs-(tspec.tv_sec*1000ul))*1000000ul;
    // Loop until the time elapses, calling again for the remainder
    // if interrupted by a signal.
    while(nanosleep(&tspec,&tspec)==-1 && errno == EINTR) {
    };
  }
  #else
  msec_t fixed_msec() {
    return static_cast<msec_t>(GetTickCount64());
  }
  msec_t msec_diff(msec_t later,msec_t earlier) {
    return (later-earlier);
  }
  void millisleep(const unsigned int millisecs) {
    Sleep(millisecs);
  }
  #endif
}

