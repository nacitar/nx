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

/// @file looper.h
/// @brief An android-esque looper.

#ifndef INCLUDE_NX_LOOPER_H_
#define INCLUDE_NX_LOOPER_H_

#include "nx/message.h"

#include <chrono>
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>

/// @brief Library namespace.
namespace nx {

class Handler;

class MessageEnvelope {
  Handler* const handler_;
  const Message message_;
 public: // for now
  MessageEnvelope(Handler* handler,Message message=Message());
  Handler* handler() const;
  const Message* message() const;
};

namespace detail {

namespace Looper {

// Due to the having bidirectional mappings from the IdMap to the QueueType, a
// forward declaration is required.
class QueueData;

typedef std::chrono::time_point<std::chrono::steady_clock> SteadyTimePoint;
// http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-defects.html#233
// as of early c++0x drafts, the language dictating behavior with duplicate
// keys has been clarified.  When inserting items of duplicate keys, they are
// added in the insertion order, so you can rely upon that in c++11.
typedef std::multimap<SteadyTimePoint, QueueData> QueueType;
typedef QueueType::iterator QueueIterator;
typedef std::multimap<unsigned int, QueueIterator> IdMapType;
typedef IdMapType::iterator IdMapIterator;

class QueueData {
 public:
  QueueData(MessageEnvelope envelope);

  MessageEnvelope envelope_;
  IdMapIterator idIterator_;
};

}  // namespace Looper

}  // namespace detail

class Looper {
  thread_local static Looper* looper_;
  std::thread::id threadId_;

  typedef detail::Looper::QueueData QueueData;
  typedef detail::Looper::QueueType QueueType;
  typedef detail::Looper::QueueIterator QueueIterator;
  typedef detail::Looper::IdMapType IdMapType;
  typedef detail::Looper::IdMapIterator IdMapIterator;

  mutable std::mutex mutex_; // mutable for hasMessages... should it be?
  std::condition_variable conditionVariable_;

  bool quit_;

 private:
  QueueType messageQueue_;
  IdMapType messageIdMap_;

  Looper();
 public:
  typedef detail::Looper::SteadyTimePoint SteadyTimePoint;

  static Looper* myLooper();

  // can be called more than once
  static bool prepare();

  std::thread::id getThreadId() const;

  void send(MessageEnvelope envelope, SteadyTimePoint triggerTime);
  void send(MessageEnvelope envelope, std::chrono::milliseconds delay);

  void remove(Handler* handler, unsigned int id,
      bool checkData = false, void* data = nullptr);
  bool hasMessages(const Handler* handler, unsigned int id,
      bool checkData = false, void* data = nullptr) const;

  static void loop();

  void runLoop();
  void quit();
};

}  // namespace nx

#endif  // INCLUDE_NX_LOOPER_H_
