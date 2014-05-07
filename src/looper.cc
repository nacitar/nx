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

/// @file looper.cc
/// @brief Implementation for looper.h

#include "nx/looper.h"
#include "nx/handler.h"

/// @brief Library namespace.
namespace nx {

MessageEnvelope::MessageEnvelope(Handler* handler, Message message)
    : handler_(handler)
    , message_(message) {
}

Handler* MessageEnvelope::handler() const {
  return handler_;
}
const Message* MessageEnvelope::message() const {
  return &message_;
}

namespace detail {

namespace Looper {

QueueData::QueueData(MessageEnvelope envelope)
  : envelope_(envelope) {
}

}  // namespace Looper

}  // namespace detail


thread_local std::shared_ptr<Looper> Looper::looper_;
Looper::Looper()
    : hasLooped_(false)
    , isQuitting_(false) {
}
std::shared_ptr<Looper> Looper::threadLooper() {
  return looper_;
}

// can be called more than once
bool Looper::prepare() {
  if (!looper_) {
    looper_.reset(new Looper());
    looper_->threadId_ = std::this_thread::get_id();
    return true;
  }
  return false;
}

std::thread::id Looper::getThreadId() const {
  return threadId_;
}
bool Looper::send(MessageEnvelope envelope, SteadyTimePoint triggerTime) {
  std::lock_guard<std::mutex> lock(mutex_);

  if (!isAlive()) return false;

  QueueIterator queueIt = messageQueue_.insert(
      QueueType::value_type(triggerTime, QueueData(envelope)));

  IdMapIterator idIt = messageIdMap_.insert(
      IdMapType::value_type(envelope.message()->id(), queueIt));

  queueIt->second.idIterator_ = idIt;

  // we need to wake up if we added this to the beginning, otherwise we're
  // already set up properly
  if (queueIt == messageQueue_.begin()) {
    conditionVariable_.notify_one();
  }

  return true;
}
bool Looper::send(MessageEnvelope envelope, std::chrono::milliseconds delay) {
  return send(envelope, std::chrono::steady_clock::now() + delay);
}

void Looper::remove(Handler* handler, unsigned int id,
    bool checkData, void* data) {
  std::lock_guard<std::mutex> lock(mutex_);

  std::pair<IdMapIterator, IdMapIterator> range =
      messageIdMap_.equal_range(id);
  while (range.first != range.second) {
    MessageEnvelope* envelope = &range.first->second->second.envelope_;
    if (envelope->handler() == handler
        && (!checkData || envelope->message()->data() == data)) {
      messageQueue_.erase(range.first->second);
      range.first = messageIdMap_.erase(range.first);
    } else {
      ++range.first;
    }
  }

  conditionVariable_.notify_one();
}

bool Looper::hasMessages(const Handler* handler, unsigned int id,
    bool checkData, void* data) {
  std::lock_guard<std::mutex> lock(mutex_);
  for (auto range = messageIdMap_.equal_range(id);
      range.first != range.second; ++range.first) {
    MessageEnvelope* envelope = &range.first->second->second.envelope_;
    if (envelope->handler() == handler
        && (!checkData || envelope->message()->data() == data)) {
      return true;
    }
  }
  return false;
}


void Looper::loop() {
  threadLooper()->runLoop();
}

bool Looper::isAlive() {
  return !isQuitting_.load() && hasLooped_.load();
}
void Looper::waitForLoop() {
  if (!hasLooped_.load()) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (!hasLooped_.load()) {
      runningConditionVariable_.wait(lock);
    }
  }
}
void Looper::runLoop() {
  using std::chrono::steady_clock;
  using std::chrono::milliseconds;
  using std::chrono::duration_cast;
  decltype(messageQueue_)::iterator it;
  SteadyTimePoint when, now;
  milliseconds delay;

  std::unique_lock<std::mutex> lock(mutex_);
  // We only allow you to loop once
  if (!isQuitting_.load()) {
    hasLooped_.store(true);
    runningConditionVariable_.notify_all();
    for ( ; !isQuitting_.load(); ) {
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
          envelope->handler()->dispatchMessage(*envelope->message());
          lock.lock();
        } else {
          conditionVariable_.wait_for(lock, delay);
        }
      } else {
        conditionVariable_.wait(lock);
      }
    }
  }
  messageQueue_.clear();
  messageIdMap_.clear();
}
void Looper::quit() {
  std::unique_lock<std::mutex> lock(mutex_);
  isQuitting_.store(true);
  conditionVariable_.notify_one();
}

}  // namespace nx

