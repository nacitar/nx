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

/// @file handler.h
/// @brief An android-esque message handler.

#ifndef INCLUDE_NX_HANDLER_H_
#define INCLUDE_NX_HANDLER_H_

#include "nx/message.h"

#include <chrono>

/// @brief Library namespace.
namespace nx {

class Looper;

class Handler {
 public:
  Looper* const looper_;
 public:
  typedef std::chrono::time_point<std::chrono::steady_clock> SteadyTimePoint;
  Handler();
  Handler(Looper*looper);
  Looper* looper();

  bool sendEmptyMessage(int id, SteadyTimePoint triggerTime);
  bool sendEmptyMessage(int id,
      std::chrono::milliseconds delay = std::chrono::milliseconds(0));

  void removeMessages(int id);

  virtual void handleMessage(Message message)=0;
};

}  // namespace nx

#endif  // INCLUDE_NX_HANDLER_H_
