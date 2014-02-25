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

#include <chrono>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>

#include "nx/message.h"

/// @brief Library namespace.
namespace nx {

class Looper;

class Handler {
 public:
  class Callback {
   public:
    /// @return True if handled and no further processing is necessary,
    /// otherwise False and the Handler's member handleMessage() will be
    /// invoked.
    virtual bool handleMessage(Message message) = 0;
  };

  Looper* const looper_;
  Callback* const callback_;

 public:
  typedef std::chrono::time_point<std::chrono::steady_clock> SteadyTimePoint;
  Handler();
  explicit Handler(Callback* callback);
  explicit Handler(Looper* looper);
  Handler(Looper* looper, Callback* callback);

  void dispatchMessage(Message message);

  Looper* looper();
  const Looper* looper() const;

  bool hasMessages(unsigned int id) const;
  bool hasMessages(unsigned int id, void* data) const;

  bool sendMessageAtFrontOfQueue(Message message);
  bool sendMessage(Message msg,
      std::chrono::milliseconds delay = std::chrono::milliseconds(0));
  bool sendMessage(Message msg, SteadyTimePoint triggerTime);
  bool sendEmptyMessage(unsigned int id,
      std::chrono::milliseconds delay = std::chrono::milliseconds(0));
  bool sendEmptyMessage(unsigned int id, SteadyTimePoint triggerTime);

  void removeMessages(unsigned int id);
  void removeMessages(unsigned int id, void* data);

  //
  virtual void handleMessage(Message message);
};

class HandlerThread {
  std::string name_;
  std::unique_ptr<std::thread> threadObject_;
  std::shared_ptr<Looper> looper_;
  std::mutex mutex_;
  std::condition_variable conditionVariable_;

  void threadFunction();

 public:
  explicit HandlerThread(const std::string& name);
  ~HandlerThread();
  /// @brief Blocks until the looper is available.
  Looper* getLooper();
  void join();
};
}  // namespace nx

#endif  // INCLUDE_NX_HANDLER_H_
