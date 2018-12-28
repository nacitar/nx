//
// Copyright (C) 2018 Jacob McIntosh <nacitar at ubercpp dot com>
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

/// @file thread_compat.h
/// @brief Supplements the std with a thread implementation for mingw.

#ifndef INCLUDE_NX_THREAD_COMPAT_H_
#define INCLUDE_NX_THREAD_COMPAT_H_

#include <thread>

#include "nx/core.h"

#ifdef NX_MINGW
// These files are 3rdparty, and they reference each other via include
// using quotations, which is compiler dependent, and without any qualifying
// path.  I'm not going to change the folder structure to appease cpplint.
#include "mingw.thread.h"  // NOLINT(build/include)
#include "mingw.mutex.h"  // NOLINT(build/include)
#include "mingw.shared_mutex.h"  // NOLINT(build/include)
#include "mingw.condition_variable.h"  // NOLINT(build/include)
#endif

#endif  // INCLUDE_NX_THREAD_COMPAT_H_
