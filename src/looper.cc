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

/// @file looper.cc
/// @brief Implementation for looper.h

#include "nx/looper.h"
#include "nx/handler.h"

/// @brief Library namespace.
namespace nx {

MessageEnvelope::MessageEnvelope(Handler* handler,
    unsigned int id, void* data)
    : Message(id, data)
    , handler_(handler) {
}

Handler* MessageEnvelope::handler() const {
  return handler_;
}

namespace detail {

namespace Looper {

QueueData::QueueData(MessageEnvelope envelope)
  : envelope_(envelope) {
}

}  // namespace Looper

}  // namespace detail


thread_local Looper* Looper::looper_;
Looper::Looper()
    : quit_(false) {
}
Looper* Looper::myLooper() {
  return looper_;
}

// can be called more than once
bool Looper::prepare() {
  if (!looper_) {
    looper_ = new Looper();
    looper_->threadId_ = std::this_thread::get_id();
    return true;
  }
  return false;
}

std::thread::id Looper::getThreadId() const {
  return threadId_;
}
void Looper::send(MessageEnvelope envelope, SteadyTimePoint triggerTime) {
  std::lock_guard<std::mutex> lock(mutex_);

  QueueIterator queueIt = messageQueue_.insert(
      QueueType::value_type(triggerTime,QueueData(envelope)));

  IdMapIterator idIt = messageIdMap_.insert(
      IdMapType::value_type(envelope.id(),queueIt));

  queueIt->second.idIterator_ = idIt;

  // we need to wake up if we added this to the beginning, otherwise we're
  // already set up properly
  if (queueIt == messageQueue_.begin()) {
    conditionVariable_.notify_one();
  }
}
void Looper::send(MessageEnvelope envelope, std::chrono::milliseconds delay) {
  send(envelope,std::chrono::steady_clock::now() + delay);
}

void Looper::remove(Handler* handler, unsigned int id,
    bool checkData, void* data) {
  std::lock_guard<std::mutex> lock(mutex_);

  std::pair<IdMapIterator, IdMapIterator> range =
      messageIdMap_.equal_range(id);
  while (range.first != range.second) {
    MessageEnvelope* envelope = &range.first->second->second.envelope_;
    if (envelope->handler() == handler
        && (!checkData || envelope->data() == data)) {
      messageQueue_.erase(range.first->second);
      range.first = messageIdMap_.erase(range.first);
    } else {
      ++range.first;
    }
  }

  conditionVariable_.notify_one();
}

bool Looper::hasMessages(const Handler* handler, unsigned int id,
    bool checkData, void* data) const {
  std::lock_guard<std::mutex> lock(mutex_);
  for(auto range = messageIdMap_.equal_range(id);
      range.first != range.second; ++range.first) {
    MessageEnvelope* envelope = &range.first->second->second.envelope_;
    if (envelope->handler() == handler
        && (!checkData || envelope->data() == data)) {
      return true;
    }
  }
  return false;
}


void Looper::loop() {
  myLooper()->runLoop();
}

void Looper::runLoop() {
  using namespace std::chrono;
  decltype(messageQueue_)::iterator it;
  SteadyTimePoint when, now;
  milliseconds delay;

  std::unique_lock<std::mutex> lock(mutex_);
  for (;!quit_;) { // infinite
    if (!messageQueue_.empty()) {
      it = messageQueue_.begin();
      MessageEnvelope* envelope = &it->second.envelope_;
      when = it->first;
      now = steady_clock::now();
      delay =
          duration_cast<milliseconds>(when - now);
      if (delay.count() <= 0) {
        // remove from queue
        messageIdMap_.erase(it->second.idIterator_);
        messageQueue_.erase(it);
        lock.unlock();
        // Calling while unlocked, because other threads can send messages
        // while we handle one.  In fact, the message handler itself may want
        // to add messages.
        envelope->handler()->dispatchMessage(*static_cast<Message*>(envelope));
        lock.lock();
      } else {
        conditionVariable_.wait_for(lock,delay);
      }
    } else {
      conditionVariable_.wait(lock);
    }
  }
  quit_ = false;
}
void Looper::quit() {
  std::unique_lock<std::mutex> lock(mutex_);
  quit_ = true;
}

}  // namespace nx

